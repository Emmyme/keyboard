#include "KeyDetector.h"
#include <algorithm>

std::vector<int> KeyDetector::hoveredKeys(const std::vector<HandLandmarks> &hands,
                                          const Calibration &calib) const
{
  std::vector<int> hovered;
  for (const auto &hand : hands)
  {
    // Single-finger pointer: only the index fingertip selects a key.
    float kx, ky;
    calib.toKeyboard(hand.points[kPointerLandmark].x, hand.points[kPointerLandmark].y, kx, ky);
    int key = layout_.keyAt(kx, ky);
    if (key >= 0 && std::find(hovered.begin(), hovered.end(), key) == hovered.end())
    {
      hovered.push_back(key);
    }
  }
  return hovered;
}

int KeyDetector::keyUnderPointer(const HandLandmarks &hand, const Calibration &calib) const
{
  float kx, ky;
  calib.toKeyboard(hand.points[kPointerLandmark].x, hand.points[kPointerLandmark].y, kx, ky);
  return layout_.keyAt(kx, ky);
}
