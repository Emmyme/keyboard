#pragma once

#include <string>
#include <vector>
#include <cstdint>

// A key as a rectangle in normalized keyboard space [0,1]x[0,1].
// (x,y) is the top-left corner, y increases downward (row 0 at the top).
struct KeyDefinition
{
  std::string label;
  uint32_t virtualKey;
  float x, y, w, h;
};

class KeyboardLayout
{
public:
  KeyboardLayout();

  void initializeDefault();
  const std::vector<KeyDefinition> &keys() const;

  // Index of the key containing (x,y) in keyboard space, or -1 if none.
  int keyAt(float x, float y) const;

private:
  std::vector<KeyDefinition> keyList;
};
