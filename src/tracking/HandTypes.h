#pragma once

#include <array>

// One hand-knuckle landmark. x,y are normalized image coordinates [0,1]
// (origin top-left, already mirror-corrected). z is relative depth (model units).
struct Landmark
{
  float x = 0.0f;
  float y = 0.0f;
  float z = 0.0f;
};

constexpr int kLandmarkCount = 21;

struct HandLandmarks
{
  std::array<Landmark, kLandmarkCount> points{};
  float score = 0.0f; // presence/confidence in [0,1]
  int handedness = -1; // 0 = left, 1 = right, -1 = unknown
};
