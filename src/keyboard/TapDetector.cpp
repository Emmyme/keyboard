#include "TapDetector.h"

std::vector<TapEvent> TapDetector::update(const std::vector<FingerSample> &fingers, double dt)
{
  std::vector<TapEvent> events;
  if (dt < 0.0) dt = 0.0;

  for (const auto &f : fingers)
  {
    State &s = states_[f.handedness];
    if (f.hoveredKey != s.key)
    {
      s.key = f.hoveredKey;
      s.dwell = 0.0;
      s.committed = false;
    }
    if (s.key >= 0 && !s.committed)
    {
      s.dwell += dt;
      if (s.dwell >= kDwellSeconds)
      {
        events.push_back({s.key, f.handedness});
        s.committed = true;
      }
    }
  }
  return events;
}

float TapDetector::progress(int handedness) const
{
  auto it = states_.find(handedness);
  if (it == states_.end() || it->second.key < 0) return 0.0f;
  if (it->second.committed) return 1.0f;
  float p = static_cast<float>(it->second.dwell / kDwellSeconds);
  return p < 0.0f ? 0.0f : (p > 1.0f ? 1.0f : p);
}
