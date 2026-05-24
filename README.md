# Laser Keyboard

A sci-fi holographic laser keyboard desktop app for Windows. The app overlays a
transparent, always-on-top window, tracks your hands from a webcam (via a Python
MediaPipe sidecar), and injects system-wide key presses as you dwell over keys.

## Project structure

- `CMakeLists.txt`: build configuration
- `assets/`: shaders and the MediaPipe hand model
- `src/`: application source code
- `scripts/tracker.py`: MediaPipe hand-tracking sidecar (streams landmarks over UDP)
- `scripts/dev-env.ps1`: sets up the MSVC + Windows SDK build environment

## Build

```powershell
. .\scripts\dev-env.ps1
cmake -B build -S . -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=cl -DCMAKE_CXX_COMPILER=cl
cmake --build build
```

The tracker needs Python with `mediapipe` and `opencv-python` installed
(`pip install --user mediapipe opencv-python`); the app spawns it automatically.

## Run

Run `build/laser_keyboard.exe` from the project root. Calibrate by holding your
index fingertip on each magenta target, then dwell over keys to type. `F8`
arms/disarms typing, `F9` recalibrates, `ESC` quits.
