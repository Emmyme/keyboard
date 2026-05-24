#include <doctest.h>
#include "keyboard/KeyboardLayout.h"

TEST_CASE("default layout has the expected letter keys")
{
  KeyboardLayout layout;
  layout.initializeDefault();
  const auto &keys = layout.keys();

  // 10 + 10 + 9 + 7 letters/digits + CAPS + BACK + space = 39
  CHECK(keys.size() == 39);

  // CAPS and BACK are present and SPACE remains the last key.
  CHECK(keys.back().label == "SPACE");
  bool hasCaps = false, hasBack = false;
  for (const auto &k : keys)
  {
    if (k.label == "CAPS") hasCaps = true;
    if (k.label == "BACK") hasBack = true;
  }
  CHECK(hasCaps);
  CHECK(hasBack);

  // Every key rect lies within keyboard space [0,1].
  for (const auto &k : keys)
  {
    CHECK(k.x >= 0.0f);
    CHECK(k.y >= 0.0f);
    CHECK(k.x + k.w <= 1.0001f);
    CHECK(k.y + k.h <= 1.0001f);
  }
}

TEST_CASE("virtual-key codes map letters to their ASCII uppercase value")
{
  KeyboardLayout layout;
  layout.initializeDefault();
  const KeyDefinition *q = nullptr;
  for (const auto &k : layout.keys())
  {
    if (k.label == "Q") { q = &k; break; }
  }
  REQUIRE(q != nullptr);
  CHECK(q->virtualKey == 'Q'); // VK for letter Q is 0x51 == 'Q'
}

TEST_CASE("keyAt returns the key under a point and -1 outside any key")
{
  KeyboardLayout layout;
  layout.initializeDefault();
  const auto &keys = layout.keys();

  // Center of the first key must hit that key.
  const auto &first = keys[0];
  int hit = layout.keyAt(first.x + first.w * 0.5f, first.y + first.h * 0.5f);
  CHECK(hit == 0);

  // A point well outside keyboard space hits nothing.
  CHECK(layout.keyAt(-1.0f, -1.0f) == -1);

  // The space bar (last key) is multi-cell-wide, so its center must resolve to it.
  int spaceIdx = static_cast<int>(keys.size()) - 1;
  const auto &space = keys[spaceIdx];
  CHECK(layout.keyAt(space.x + space.w * 0.5f, space.y + space.h * 0.5f) == spaceIdx);

  // keyAt round-trips with the stored rect for a specific letter found by label.
  int qIdx = -1;
  for (int i = 0; i < static_cast<int>(keys.size()); ++i)
  {
    if (keys[i].label == "Q") { qIdx = i; break; }
  }
  REQUIRE(qIdx != -1);
  const auto &q = keys[qIdx];
  CHECK(layout.keyAt(q.x + q.w * 0.5f, q.y + q.h * 0.5f) == qIdx);
}
