#include <doctest.h>
#include "mouse/PoseStabilizer.h"

TEST_CASE("a new pose held past the confirm time becomes the reported pose")
{
  PoseStabilizer s;
  CHECK(s.update(Pose::PinchIndex, 0.05) == Pose::None);       // 0.05, still confirming
  CHECK(s.update(Pose::PinchIndex, 0.05) == Pose::PinchIndex); // 0.10, confirmed
}

TEST_CASE("a brief one-frame flicker does not change the reported pose")
{
  PoseStabilizer s;
  for (int i = 0; i < 5; ++i) s.update(Pose::Point, 0.05); // settle on Point
  CHECK(s.update(Pose::Point, 0.05) == Pose::Point);
  CHECK(s.update(Pose::PinchIndex, 0.05) == Pose::Point); // single flicker ignored
  CHECK(s.update(Pose::Point, 0.05) == Pose::Point);
}

TEST_CASE("rapid alternating flicker never confirms a new pose")
{
  PoseStabilizer s;
  for (int i = 0; i < 5; ++i) s.update(Pose::Point, 0.05); // settle on Point
  for (int i = 0; i < 10; ++i)
  {
    CHECK(s.update(Pose::PinchIndex, 0.05) == Pose::Point);
    CHECK(s.update(Pose::Point, 0.05) == Pose::Point);
  }
}

TEST_CASE("a deliberate sustained pose change is reported")
{
  PoseStabilizer s;
  for (int i = 0; i < 5; ++i) s.update(Pose::Point, 0.05); // settle on Point
  Pose last = Pose::Point;
  for (int i = 0; i < 5; ++i) last = s.update(Pose::PinchIndex, 0.05);
  CHECK(last == Pose::PinchIndex);
}
