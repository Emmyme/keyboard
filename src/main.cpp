#include "window/OverlayWindow.h"
#include "renderer/Renderer.h"
#include "keyboard/KeyboardLayout.h"
#include "keyboard/KeyDetector.h"
#include "keyboard/Calibrator.h"
#include "keyboard/TapDetector.h"
#include "config/Config.h"
#include "input/LandmarkReceiver.h"
#include "input/KeyInjector.h"
#include "tracker_paths.h" // baked LK_PYTHON_EXE / LK_PROJECT_ROOT
#include <windows.h>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace
{
// First hand's index fingertip in image space, or NaN if no hand.
void activeFingertip(const std::vector<HandLandmarks> &hands, float &x, float &y)
{
  if (hands.empty()) { x = y = std::nanf(""); return; }
  x = hands[0].points[8].x;
  y = hands[0].points[8].y;
}
}

int main()
{
  OverlayWindow overlay;
  if (!overlay.initialize("Laser Keyboard")) return EXIT_FAILURE;

  Config config;
  config.loadDefault();
  config.loadFromFile("assets/config.json");

  KeyboardLayout layout;
  layout.initializeDefault();
  KeyDetector detector(layout);

  Renderer renderer;
  if (!renderer.initialize(overlay.framebufferWidth(), overlay.framebufferHeight()))
    return EXIT_FAILURE;

  // Spawn the Python MediaPipe tracker. Absolute python + explicit working dir
  // (baked at configure time) make this independent of the launch shell's PATH/cwd.
  STARTUPINFOA si{};
  si.cb = sizeof(si);
  PROCESS_INFORMATION pi{};
  std::string pyExe = LK_PYTHON_EXE;
  std::string workDir = LK_PROJECT_ROOT;
  for (char &c : pyExe) if (c == '/') c = '\\';
  for (char &c : workDir) if (c == '/') c = '\\';
  std::string cmdStr = "\"" + pyExe + "\" scripts\\tracker.py";
  std::vector<char> cmdBuf(cmdStr.begin(), cmdStr.end());
  cmdBuf.push_back('\0');
  bool spawned = CreateProcessA(pyExe.c_str(), cmdBuf.data(), nullptr, nullptr, FALSE,
                                CREATE_NO_WINDOW, nullptr, workDir.c_str(), &si, &pi);
  if (!spawned)
    std::cerr << "WARN: could not spawn tracker (" << pyExe << "), err=" << GetLastError()
              << "; run it manually: python scripts\\tracker.py\n";

  LandmarkReceiver tracker;
  if (!tracker.start(51234))
  {
    std::cerr << "ERROR: could not open landmark UDP socket\n";
    return EXIT_FAILURE;
  }

  Calibration calib;
  Calibrator calibrator;
  TapDetector tapDetector;
  KeyInjector injector;
  injector.setArmed(true); // armed on start: a committed dwell types immediately
  std::cout << "Calibrate: move your index fingertip to each magenta target and hold.\n"
            << "Typing is ON at start. Dwell CAPS for uppercase.\n"
            << "Keys: F8 arm/disarm, F9 recalibrate, ESC quit." << std::endl;

  // Locate the on-screen CAPS toggle (sentinel VK_CAPITAL) once.
  int capsKey = -1;
  for (int i = 0; i < static_cast<int>(layout.keys().size()); ++i)
    if (layout.keys()[i].virtualKey == VK_CAPITAL) capsKey = i;
  bool capsOn = false;

  bool prevF9 = false, prevF8 = false;
  auto last = std::chrono::steady_clock::now();

  while (overlay.processEvents())
  {
    if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;

    bool f9 = (GetAsyncKeyState(VK_F9) & 0x8000) != 0;
    if (f9 && !prevF9) { calibrator.reset(); calib.valid = false; }
    prevF9 = f9;

    bool f8 = (GetAsyncKeyState(VK_F8) & 0x8000) != 0;
    if (f8 && !prevF8)
    {
      injector.toggle();
      std::cout << "Injection " << (injector.armed() ? "ARMED" : "disarmed") << std::endl;
    }
    prevF8 = f8;

    auto now = std::chrono::steady_clock::now();
    double dt = std::chrono::duration<double>(now - last).count();
    last = now;

    auto hands = tracker.latest();

    std::vector<int> hovered, pressed;
    std::vector<std::pair<float, float>> fingertipsKb, targetsKb;

    if (calibrator.state() != Calibrator::State::Done)
    {
      float fx, fy;
      activeFingertip(hands, fx, fy);
      calibrator.update(fx, fy, dt, calib);

      bool corner0 = calibrator.state() == Calibrator::State::Corner0;
      targetsKb.push_back({corner0 ? Calibrator::kTarget0X : Calibrator::kTarget1X,
                           corner0 ? Calibrator::kTarget0Y : Calibrator::kTarget1Y});
      if (std::isfinite(fx)) fingertipsKb.push_back({fx, fy}); // identity during calib
    }
    else
    {
      hovered = detector.hoveredKeys(hands, calib);
      for (const auto &hand : hands)
      {
        float kx, ky;
        calib.toKeyboard(hand.points[kPointerLandmark].x, hand.points[kPointerLandmark].y, kx, ky);
        fingertipsKb.push_back({kx, ky});
      }

      // Dwell-to-type runs on the render dt: dwell sums to wall time, and the
      // hovered key is stable between tracker updates.
      std::vector<FingerSample> fingers;
      for (const auto &hand : hands)
        fingers.push_back({hand.handedness, detector.keyUnderPointer(hand, calib)});
      for (const auto &tap : tapDetector.update(fingers, dt))
      {
        if (tap.key < 0 || tap.key >= static_cast<int>(layout.keys().size())) continue;
        uint32_t vk = layout.keys()[tap.key].virtualKey;
        if (vk == VK_CAPITAL) { capsOn = !capsOn; continue; } // toggle, never inject
        bool shift = capsOn && vk >= 'A' && vk <= 'Z';        // caps affects letters only
        injector.tap(vk, shift);                              // no-op unless armed
      }

      // A key fills toward 'pressed' as you dwell on it (>50% of the dwell time).
      for (const auto &hand : hands)
      {
        int key = detector.keyUnderPointer(hand, calib);
        if (key >= 0 && tapDetector.progress(hand.handedness) > 0.5f)
          pressed.push_back(key);
      }
      if (capsOn && capsKey >= 0) pressed.push_back(capsKey); // stays lit while active
    }

    renderer.renderFrame(layout, hovered, pressed, config, fingertipsKb, targetsKb,
                         injector.armed());
    overlay.swapBuffers();
  }

  tracker.stop();
  if (spawned)
  {
    TerminateProcess(pi.hProcess, 0);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
  }
  return 0;
}
