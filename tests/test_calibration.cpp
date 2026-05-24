#include <doctest.h>
#include "keyboard/Calibration.h"
#include "keyboard/KeyDetector.h"
#include "keyboard/KeyboardLayout.h"
#include <algorithm>

TEST_CASE("calibration maps reach-rect corners to keyboard [0,1]")
{
  Calibration c;
  c.x0 = 0.2f; c.y0 = 0.2f; c.x1 = 0.8f; c.y1 = 0.8f;
  float kx, ky;
  c.toKeyboard(0.2f, 0.2f, kx, ky);
  CHECK(kx == doctest::Approx(0.0f));
  CHECK(ky == doctest::Approx(0.0f));
  c.toKeyboard(0.8f, 0.8f, kx, ky);
  CHECK(kx == doctest::Approx(1.0f));
  CHECK(ky == doctest::Approx(1.0f));
  c.toKeyboard(0.5f, 0.5f, kx, ky);
  CHECK(kx == doctest::Approx(0.5f));
}

TEST_CASE("hoveredKeys finds the key under a fingertip")
{
  KeyboardLayout layout;
  layout.initializeDefault();
  KeyDetector det(layout);

  Calibration c; // identity: image [0,1] -> keyboard [0,1]
  c.x0 = 0.0f; c.y0 = 0.0f; c.x1 = 1.0f; c.y1 = 1.0f;

  const auto &k0 = layout.keys()[0];
  HandLandmarks hand;
  hand.points[8].x = k0.x + k0.w * 0.5f;
  hand.points[8].y = k0.y + k0.h * 0.5f;

  auto hovered = det.hoveredKeys({hand}, c);
  REQUIRE(!hovered.empty());
  CHECK(std::find(hovered.begin(), hovered.end(), 0) != hovered.end());
}
