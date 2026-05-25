#include <doctest.h>
#include "mouse/PalmToggle.h"

TEST_CASE("an open palm held for the dwell time fires once")
{
  PalmToggle t;
  CHECK_FALSE(t.update(true, 0.4));
  CHECK_FALSE(t.update(true, 0.3)); // 0.7 total, under dwell
  CHECK(t.update(true, 0.2));       // 0.9 total, fires
}

TEST_CASE("it does not re-fire while still held (cooldown)")
{
  PalmToggle t;
  CHECK(t.update(true, 1.0)); // fires
  CHECK_FALSE(t.update(true, 0.2));
  CHECK_FALSE(t.update(true, 0.2));
}

TEST_CASE("releasing resets so a later hold fires again")
{
  PalmToggle t;
  CHECK(t.update(true, 1.0)); // fires, enters cooldown
  CHECK_FALSE(t.update(false, 0.1)); // release ends cooldown
  CHECK_FALSE(t.update(true, 0.5));
  CHECK(t.update(true, 0.5)); // 1.0 held, fires again
}
