#pragma once

#include "mouse/HandPose.h"
#include <vector>

enum class MouseIntentType { MoveBy, LeftDown, LeftUp };

struct MouseIntent
{
  MouseIntentType type;
  float dx = 0.0f; // MoveBy, pixels
  float dy = 0.0f; // MoveBy, pixels
};

// Turns a per-frame (pose, hand centroid, dt) stream into mouse intents. Pure
// and time-driven, no OS calls, so it is fully unit-testable. Gesture set: Point
// moves the cursor and PinchIndex fires one left click on entry. The pose is
// expected to be debounced upstream (see PoseStabilizer).
class MouseController
{
public:
  std::vector<MouseIntent> update(Pose pose, float centerX, float centerY, double dt);
  void reset();

private:
  Pose prevPose_ = Pose::None;
  bool hasPrev_ = false;
  float prevX_ = 0.0f, prevY_ = 0.0f;
  float emaDx_ = 0.0f, emaDy_ = 0.0f;
};
