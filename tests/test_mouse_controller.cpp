#include <doctest.h>
#include "mouse/MouseController.h"

static const MouseIntent *find(const std::vector<MouseIntent> &v, MouseIntentType t)
{
  for (const auto &i : v)
    if (i.type == t) return &i;
  return nullptr;
}

static int count(const std::vector<MouseIntent> &v, MouseIntentType t)
{
  int n = 0;
  for (const auto &i : v)
    if (i.type == t) ++n;
  return n;
}

TEST_CASE("first Point frame establishes a reference and does not move")
{
  MouseController c;
  auto out = c.update(Pose::Point, 0.50f, 0.50f, 0.016);
  CHECK(find(out, MouseIntentType::MoveBy) == nullptr);
}

TEST_CASE("a second Point frame moves by the gained delta")
{
  MouseController c;
  c.update(Pose::Point, 0.50f, 0.50f, 0.016);
  auto out = c.update(Pose::Point, 0.55f, 0.50f, 0.016);
  const MouseIntent *m = find(out, MouseIntentType::MoveBy);
  REQUIRE(m != nullptr);
  CHECK(m->dx > 0.0f);
  CHECK(m->dy == doctest::Approx(0.0f).epsilon(0.001));
}

TEST_CASE("changing pose clutches, so re-entering Point does not jump")
{
  MouseController c;
  c.update(Pose::Point, 0.20f, 0.20f, 0.016);
  c.update(Pose::None, 0.20f, 0.20f, 0.016);
  auto out = c.update(Pose::Point, 0.80f, 0.80f, 0.016);
  CHECK(find(out, MouseIntentType::MoveBy) == nullptr);
}

TEST_CASE("entering a pinch fires exactly one left click")
{
  MouseController c;
  auto a = c.update(Pose::PinchIndex, 0.5f, 0.5f, 0.016);
  CHECK(count(a, MouseIntentType::LeftDown) == 1);
  CHECK(count(a, MouseIntentType::LeftUp) == 1);
  auto b = c.update(Pose::PinchIndex, 0.5f, 0.5f, 0.016); // held, no repeat
  CHECK(count(b, MouseIntentType::LeftDown) == 0);
}

TEST_CASE("releasing a pinch emits nothing")
{
  MouseController c;
  c.update(Pose::PinchIndex, 0.5f, 0.5f, 0.016);
  auto rel = c.update(Pose::None, 0.55f, 0.5f, 0.016);
  CHECK(rel.empty());
}

TEST_CASE("pinch then pinch again fires a second click")
{
  MouseController c;
  CHECK(count(c.update(Pose::PinchIndex, 0.5f, 0.5f, 0.016), MouseIntentType::LeftDown) == 1);
  c.update(Pose::Point, 0.5f, 0.5f, 0.016); // leave pinch
  CHECK(count(c.update(Pose::PinchIndex, 0.5f, 0.5f, 0.016), MouseIntentType::LeftDown) == 1);
}

