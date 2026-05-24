#include <doctest.h>
#include "keyboard/TapDetector.h"

// Tests are written relative to kDwellSeconds so tuning it doesn't break them.
static const double D = TapDetector::kDwellSeconds;

TEST_CASE("dwell commits once after holding the dwell time")
{
  TapDetector det;
  CHECK(det.update({{1, 5}}, D * 0.4).empty());
  CHECK(det.update({{1, 5}}, D * 0.4).empty()); // 0.8D total, < D
  auto taps = det.update({{1, 5}}, D * 0.3);    // 1.1D -> commit
  REQUIRE(taps.size() == 1);
  CHECK(taps[0].key == 5);
  CHECK(taps[0].handedness == 1);
  CHECK(det.update({{1, 5}}, D * 0.4).empty()); // still holding -> no repeat
}

TEST_CASE("moving off a key resets the dwell")
{
  TapDetector det;
  det.update({{0, 3}}, D * 0.7);
  det.update({{0, -1}}, D * 0.2);                  // moved off -> reset
  CHECK(det.update({{0, 3}}, D * 0.7).empty());    // back on, 0.7D
  CHECK(det.update({{0, 3}}, D * 0.5).size() == 1); // 1.2D -> commit
}

TEST_CASE("re-commit after leaving and returning")
{
  TapDetector det;
  CHECK(det.update({{1, 7}}, D * 1.2).size() == 1); // commit
  CHECK(det.update({{1, 7}}, D * 0.3).empty());      // holding -> no repeat
  det.update({{1, -1}}, D * 0.2);                    // leave
  CHECK(det.update({{1, 7}}, D * 1.2).size() == 1);  // return + dwell -> commit again
}

TEST_CASE("progress reflects dwell fraction")
{
  TapDetector det;
  det.update({{0, 2}}, D * 0.5);
  CHECK(det.progress(0) == doctest::Approx(0.5f).epsilon(0.02));
  det.update({{0, 2}}, D); // committed
  CHECK(det.progress(0) == doctest::Approx(1.0f));
}
