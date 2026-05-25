#include "HandPose.h"
#include <cmath>

namespace
{
constexpr float kPinchThreshold = 0.35f; // normalized by hand size

float dist(const Landmark &a, const Landmark &b)
{
  float dx = a.x - b.x;
  float dy = a.y - b.y;
  return std::sqrt(dx * dx + dy * dy);
}

// A finger is extended when its tip is farther from the wrist than its mid joint.
bool extended(const HandLandmarks &h, int tip, int pip)
{
  return dist(h.points[tip], h.points[0]) > dist(h.points[pip], h.points[0]);
}
} // namespace

Pose classifyPose(const HandLandmarks &h)
{
  const float handSize = dist(h.points[0], h.points[9]);
  if (handSize < 1e-6f) return Pose::None; // degenerate

  if (dist(h.points[4], h.points[8]) / handSize < kPinchThreshold) return Pose::PinchIndex;

  const bool thumb = extended(h, 4, 3);
  const bool idx = extended(h, 8, 6);
  const bool mid = extended(h, 12, 10);
  const bool ring = extended(h, 16, 14);
  const bool pinky = extended(h, 20, 18);

  if (thumb && idx && mid && ring && pinky) return Pose::OpenPalm;
  if (idx && !mid && !ring && !pinky) return Pose::Point;
  return Pose::None;
}
