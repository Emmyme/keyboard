#pragma once

#include <string>
#include <array>

using Color = std::array<float, 3>; // r, g, b in [0,1]

struct Config
{
  // Key glow theme.
  Color idleColor{0.0f, 0.55f, 0.75f};
  Color hoverColor{0.2f, 1.0f, 1.0f};
  Color pressColor{1.0f, 0.4f, 1.0f};
  float idleIntensity = 0.55f;
  float hoverIntensity = 1.0f;
  float pressIntensity = 1.6f;

  void loadDefault();                          // reset to defaults above
  bool loadFromFile(const std::string &path);  // overlay values, false if no file
};
