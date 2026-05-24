#pragma once

#include "keyboard/KeyboardLayout.h"
#include "keyboard/Calibration.h"
#include "tracking/HandTypes.h"
#include <vector>

// Single-finger pointer: the index fingertip drives hover/typing ("hunt and peck").
constexpr int kPointerLandmark = 8;

class KeyDetector
{
public:
  explicit KeyDetector(const KeyboardLayout &layout) : layout_(layout) {}

  // Key indices currently hovered by any hand's pointer (deduplicated).
  std::vector<int> hoveredKeys(const std::vector<HandLandmarks> &hands,
                               const Calibration &calib) const;
  // The key under one hand's index fingertip, or -1.
  int keyUnderPointer(const HandLandmarks &hand, const Calibration &calib) const;

private:
  const KeyboardLayout &layout_;
};
