#pragma once

#include <unordered_map>
#include <vector>

// One hand's current pointer state.
struct FingerSample
{
  int handedness; // stable key across frames (0/1)
  int hoveredKey; // key index under the pointer, or -1
};

struct TapEvent
{
  int key;
  int handedness;
};

// Dwell-to-type: holding the pointer over a key for kDwellSeconds commits it
// once. Moving off resets, and returning lets it commit again. Uses only x/y
// (the stable signals), no depth.
class TapDetector
{
public:
  static constexpr double kDwellSeconds = 1.0; // hold time to commit a key

  std::vector<TapEvent> update(const std::vector<FingerSample> &fingers, double dt);
  // Dwell fill [0,1] for a hand's current key (for UI feedback), 0 if none.
  float progress(int handedness) const;

private:
  struct State
  {
    int key = -1;
    double dwell = 0.0;
    bool committed = false;
  };
  std::unordered_map<int, State> states_;
};
