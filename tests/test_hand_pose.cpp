#include <doctest.h>
#include "mouse/HandPose.h"

// Build a hand with each finger either curled or extended. Fingers occupy
// separate x columns so non-pinch poses do not read as a pinch. y grows
// downward, so an extended fingertip sits high (small y), far from the wrist.
namespace
{
constexpr float kColX[5] = {0.35f, 0.45f, 0.50f, 0.57f, 0.64f}; // thumb..pinky
constexpr int kTip[5] = {4, 8, 12, 16, 20};
constexpr int kPip[5] = {3, 6, 10, 14, 18};

HandLandmarks baseHand()
{
  HandLandmarks h;
  h.points[0] = {0.50f, 0.90f, 0.0f}; // wrist
  h.points[9] = {0.50f, 0.65f, 0.0f}; // middle MCP, handSize 0.25
  for (int f = 0; f < 5; ++f)
  {
    h.points[kPip[f]] = {kColX[f], 0.60f, 0.0f};
    h.points[kTip[f]] = {kColX[f], 0.66f, 0.0f}; // curled: tip near palm
  }
  return h;
}
void extend(HandLandmarks &h, int f)
{
  h.points[kPip[f]] = {kColX[f], 0.45f, 0.0f};
  h.points[kTip[f]] = {kColX[f], 0.30f, 0.0f};
}
} // namespace

TEST_CASE("a closed fist is None")
{
  CHECK(classifyPose(baseHand()) == Pose::None);
}

TEST_CASE("index only is Point")
{
  HandLandmarks h = baseHand();
  extend(h, 1);
  CHECK(classifyPose(h) == Pose::Point);
}

TEST_CASE("index and middle together is not Point")
{
  HandLandmarks h = baseHand();
  extend(h, 1);
  extend(h, 2);
  CHECK(classifyPose(h) == Pose::None);
}

TEST_CASE("all five extended is OpenPalm")
{
  HandLandmarks h = baseHand();
  for (int f = 0; f < 5; ++f) extend(h, f);
  CHECK(classifyPose(h) == Pose::OpenPalm);
}

TEST_CASE("thumb touching index tip is PinchIndex, beating Point")
{
  HandLandmarks h = baseHand();
  extend(h, 1);
  h.points[4] = h.points[8]; // thumb tip onto index tip
  CHECK(classifyPose(h) == Pose::PinchIndex);
}
