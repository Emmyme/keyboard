#include <doctest.h>
#include "input/MouseInjector.h"
#include <vector>

// A fake backend records events instead of touching the real cursor.
static MouseInjector recording(std::vector<MouseEvent> &log)
{
  return MouseInjector([&log](const MouseEvent &e) { log.push_back(e); return true; });
}

TEST_CASE("mouse injector is disarmed by default and sends nothing")
{
  std::vector<MouseEvent> log;
  MouseInjector inj = recording(log);
  CHECK_FALSE(inj.armed());
  CHECK_FALSE(inj.leftDown());
  CHECK(log.empty());
}

TEST_CASE("left down and up send the matching events when armed")
{
  std::vector<MouseEvent> log;
  MouseInjector inj = recording(log);
  inj.setArmed(true);
  CHECK(inj.leftDown());
  CHECK(inj.leftUp());
  REQUIRE(log.size() == 2);
  CHECK(log[0].action == MouseAction::LeftDown);
  CHECK(log[1].action == MouseAction::LeftUp);
}

TEST_CASE("move forwards the delta when armed")
{
  std::vector<MouseEvent> log;
  MouseInjector inj = recording(log);
  inj.setArmed(true);
  CHECK(inj.moveBy(7, -3));
  REQUIRE(log.size() == 1);
  CHECK(log[0].action == MouseAction::Move);
  CHECK(log[0].dx == 7);
  CHECK(log[0].dy == -3);
}

TEST_CASE("move with zero delta sends nothing")
{
  std::vector<MouseEvent> log;
  MouseInjector inj = recording(log);
  inj.setArmed(true);
  CHECK_FALSE(inj.moveBy(0, 0));
  CHECK(log.empty());
}

TEST_CASE("clampToScreen keeps the point inside the screen rect")
{
  int x = 0, y = 0;
  clampToScreen(10, 10, -50, 5, 800, 600, x, y);
  CHECK(x == 0);
  CHECK(y == 15);
  clampToScreen(790, 590, 100, 100, 800, 600, x, y);
  CHECK(x == 799);
  CHECK(y == 599);
}
