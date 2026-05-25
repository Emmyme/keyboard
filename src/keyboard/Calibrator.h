#pragma once

#include "keyboard/Calibration.h"

// Dwell-based 2-corner capture. The app shows a target at a screen corner, the
// user moves a fingertip to it and holds still, and we capture the fingertip's
// image-space position. Two captures define the reach rectangle.
class Calibrator
{
public:
  enum class State { Corner0, Corner1, Done };

  // Targets in normalized image space the user should reach toward (drawn on screen).
  static constexpr float kTarget0X = 0.30f, kTarget0Y = 0.30f;
  static constexpr float kTarget1X = 0.70f, kTarget1Y = 0.70f;

  void reset();
  // Feed the active fingertip image position each frame (or NaN if none).
  // dtSeconds advances the dwell timer. Returns true when calibration completes.
  bool update(float fingertipX, float fingertipY, double dtSeconds, Calibration &outCalib);

  State state() const { return state_; }

private:
  static constexpr double kDwellNeeded = 1.0; // seconds to hold
  static constexpr float kDwellRadius = 0.06f; // image-space proximity to target

  State state_ = State::Corner0;
  double dwellSeconds_ = 0.0;
  float captured0X_ = 0.0f, captured0Y_ = 0.0f;
};
