#include "MouseController.h"

namespace
{
constexpr float kMoveGain = 1600.0f; // image-space delta to pixels
constexpr float kSmoothAlpha = 0.5f; // EMA factor on the move delta
} // namespace

std::vector<MouseIntent> MouseController::update(Pose pose, float cx, float cy, double dt)
{
  std::vector<MouseIntent> out;
  if (dt < 0.0) dt = 0.0;

  const bool poseChanged = pose != prevPose_;

  // A debounced pinch fires exactly one left click on entry.
  if (poseChanged && pose == Pose::PinchIndex)
  {
    out.push_back({MouseIntentType::LeftDown});
    out.push_back({MouseIntentType::LeftUp});
  }

  // Any pose change resets the relative-motion reference (the clutch).
  if (poseChanged)
  {
    hasPrev_ = false;
    emaDx_ = 0.0f;
    emaDy_ = 0.0f;
  }

  float dx = 0.0f, dy = 0.0f;
  if (hasPrev_)
  {
    dx = cx - prevX_;
    dy = cy - prevY_;
  }
  prevX_ = cx;
  prevY_ = cy;
  hasPrev_ = true;

  if (pose == Pose::Point)
  {
    emaDx_ = kSmoothAlpha * dx + (1.0f - kSmoothAlpha) * emaDx_;
    emaDy_ = kSmoothAlpha * dy + (1.0f - kSmoothAlpha) * emaDy_;
    if (emaDx_ != 0.0f || emaDy_ != 0.0f)
      out.push_back({MouseIntentType::MoveBy, emaDx_ * kMoveGain, emaDy_ * kMoveGain});
  }

  prevPose_ = pose;
  return out;
}

void MouseController::reset() { *this = MouseController{}; }
