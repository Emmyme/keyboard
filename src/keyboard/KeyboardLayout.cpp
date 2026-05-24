#include "KeyboardLayout.h"
#include <Windows.h>

namespace
{
constexpr int kCols = 10; // widest row drives the cell grid
constexpr int kRows = 5;  // digits + 3 letter rows + space

uint32_t vkFor(char c)
{
  return static_cast<uint32_t>(VkKeyScanA(c) & 0xFF);
}
}

KeyboardLayout::KeyboardLayout() = default;

void KeyboardLayout::initializeDefault()
{
  keyList.clear();
  const float cellW = 1.0f / kCols;
  const float cellH = 1.0f / kRows;

  const char *rows[4] = {"1234567890", "QWERTYUIOP", "ASDFGHJKL", "ZXCVBNM"};

  for (int r = 0; r < 4; ++r)
  {
    const char *row = rows[r];
    int len = 0;
    while (row[len]) ++len;
    const float indent = (kCols - len) * 0.5f * cellW; // center shorter rows
    const float y = r * cellH;
    for (int i = 0; i < len; ++i)
    {
      keyList.push_back({std::string(1, row[i]), vkFor(row[i]),
                         indent + i * cellW, y, cellW, cellH});
    }
  }

  // CAPS toggle filling the left gap of the bottom letter row (like a real
  // keyboard's Shift). VK_CAPITAL is a sentinel the app treats as a local
  // uppercase toggle. It is never injected.
  keyList.push_back({"CAPS", VK_CAPITAL, 0.0f, 3 * cellH, 1.5f * cellW, cellH});

  // BACK (backspace) mirrors CAPS in the right gap of the bottom letter row.
  keyList.push_back({"BACK", VK_BACK, 8.5f * cellW, 3 * cellH, 1.5f * cellW, cellH});

  // Space bar on the bottom row, spanning the middle 6 cells.
  keyList.push_back({"SPACE", VK_SPACE, 2.0f * cellW, 4 * cellH, 6.0f * cellW, cellH});
}

const std::vector<KeyDefinition> &KeyboardLayout::keys() const
{
  return keyList;
}

int KeyboardLayout::keyAt(float x, float y) const
{
  for (int i = 0; i < static_cast<int>(keyList.size()); ++i)
  {
    const auto &k = keyList[i];
    // Half-open on the far edges so tiled keys don't both claim a shared boundary.
    if (x >= k.x && x < k.x + k.w && y >= k.y && y < k.y + k.h)
    {
      return i;
    }
  }
  return -1;
}
