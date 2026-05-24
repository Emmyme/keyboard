#include "Calibrator.h"
#include <algorithm>
#include <cmath>

void Calibrator::reset()
{
  state_ = State::Corner0;
  dwellSeconds_ = 0.0;
}

bool Calibrator::update(float fx, float fy, double dt, Calibration &outCalib)
{
  if (state_ == State::Done) return true;

  float tx = state_ == State::Corner0 ? kTarget0X : kTarget1X;
  float ty = state_ == State::Corner0 ? kTarget0Y : kTarget1Y;

  bool near = std::isfinite(fx) && std::isfinite(fy) &&
              std::hypot(fx - tx, fy - ty) < kDwellRadius;
  dwellSeconds_ = near ? dwellSeconds_ + dt : 0.0;

  if (dwellSeconds_ >= kDwellNeeded)
  {
    dwellSeconds_ = 0.0;
    if (state_ == State::Corner0)
    {
      captured0X_ = fx;
      captured0Y_ = fy;
      state_ = State::Corner1;
    }
    else
    {
      outCalib.x0 = std::min(captured0X_, fx);
      outCalib.y0 = std::min(captured0Y_, fy);
      outCalib.x1 = std::max(captured0X_, fx);
      outCalib.y1 = std::max(captured0Y_, fy);
      outCalib.valid = true;
      state_ = State::Done;
      return true;
    }
  }
  return false;
}
