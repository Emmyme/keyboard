#pragma once

// Open-palm dwell detector for switching modes. Returns true once when an open
// palm has been held for kDwell, then enters a short cooldown so a single hold
// toggles exactly once. Releasing the palm ends the cooldown early.
class PalmToggle
{
public:
  static constexpr double kDwell = 0.8;
  static constexpr double kCooldown = 0.6;

  bool update(bool isOpenPalm, double dt)
  {
    if (dt < 0.0) dt = 0.0;
    if (cooldown_ > 0.0)
    {
      cooldown_ -= dt;
      if (!isOpenPalm) cooldown_ = 0.0;
      held_ = 0.0;
      return false;
    }
    if (isOpenPalm)
    {
      held_ += dt;
      if (held_ >= kDwell)
      {
        held_ = 0.0;
        cooldown_ = kCooldown;
        return true;
      }
    }
    else
    {
      held_ = 0.0;
    }
    return false;
  }

private:
  double held_ = 0.0;
  double cooldown_ = 0.0;
};
