#pragma once

#include "renderer/ShaderProgram.h"
#include "keyboard/KeyboardLayout.h"
#include "config/Config.h"
#include <utility>
#include <vector>

// NDC sub-region the keyboard is drawn into.
struct NdcRegion
{
  float xMin, yMin, xMax, yMax;
};

// Per-key state: 0 = idle, 1 = hover, 2 = pressed.
class KeyRenderer
{
public:
  KeyRenderer();
  ~KeyRenderer();

  bool initialize();
  void draw(const std::vector<KeyDefinition> &keys, const std::vector<unsigned char> &states,
            const Config &config, const NdcRegion &region);
  // Draws small glowing dots at the given NDC positions (fingertips / targets).
  void drawDots(const std::vector<std::pair<float, float>> &ndcPoints,
                float r, float g, float b, float sizeNdc);

private:
  ShaderProgram shader;
  unsigned int vao = 0;
  unsigned int vbo = 0;
};
