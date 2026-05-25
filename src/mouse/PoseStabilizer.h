#pragma once

#include "mouse/HandPose.h"

// Debounces the jittery per-frame pose stream: a new pose must persist for
// kConfirm before it is reported, so brief misclassifications fire no actions.
class PoseStabilizer
{
public:
  static constexpr double kConfirm = 0.10; // seconds a new pose must hold

  Pose update(Pose raw, double dt)
  {
    if (dt < 0.0) dt = 0.0;
    if (raw == confirmed_)
    {
      candidate_ = raw;
      candidateTime_ = 0.0;
      return confirmed_;
    }
    if (raw == candidate_)
    {
      candidateTime_ += dt;
      if (candidateTime_ >= kConfirm)
      {
        confirmed_ = raw;
        candidateTime_ = 0.0;
      }
    }
    else
    {
      candidate_ = raw;
      candidateTime_ = dt;
    }
    return confirmed_;
  }

private:
  Pose confirmed_ = Pose::None;
  Pose candidate_ = Pose::None;
  double candidateTime_ = 0.0;
};
