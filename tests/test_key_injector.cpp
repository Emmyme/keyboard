#include <doctest.h>
#include "input/KeyInjector.h"
#include <utility>
#include <vector>

// A fake sender records (virtualKey, shift) instead of touching the real OS, so
// these tests never inject keystrokes into the machine running them.

TEST_CASE("disarmed by default and injects nothing")
{
  std::vector<std::pair<uint32_t, bool>> sent;
  KeyInjector inj([&](uint32_t vk, bool shift) { sent.push_back({vk, shift}); return true; });
  CHECK_FALSE(inj.armed());
  CHECK_FALSE(inj.tap(0x41)); // 'A'
  CHECK(sent.empty());
}

TEST_CASE("sends only when armed")
{
  std::vector<std::pair<uint32_t, bool>> sent;
  KeyInjector inj([&](uint32_t vk, bool shift) { sent.push_back({vk, shift}); return true; });
  inj.setArmed(true);
  CHECK(inj.tap(0x41));
  REQUIRE(sent.size() == 1);
  CHECK(sent[0].first == 0x41);
  CHECK(sent[0].second == false);
}

TEST_CASE("forwards the shift modifier for uppercase")
{
  std::vector<std::pair<uint32_t, bool>> sent;
  KeyInjector inj([&](uint32_t vk, bool shift) { sent.push_back({vk, shift}); return true; });
  inj.setArmed(true);
  CHECK(inj.tap(0x41, true));
  REQUIRE(sent.size() == 1);
  CHECK(sent[0].first == 0x41);
  CHECK(sent[0].second == true);
}

TEST_CASE("ignores virtual-key 0 even when armed")
{
  std::vector<std::pair<uint32_t, bool>> sent;
  KeyInjector inj([&](uint32_t vk, bool shift) { sent.push_back({vk, shift}); return true; });
  inj.setArmed(true);
  CHECK_FALSE(inj.tap(0));
  CHECK(sent.empty());
}

TEST_CASE("toggle flips the armed state")
{
  KeyInjector inj([](uint32_t, bool) { return true; });
  CHECK_FALSE(inj.armed());
  inj.toggle();
  CHECK(inj.armed());
  inj.toggle();
  CHECK_FALSE(inj.armed());
}
