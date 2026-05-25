#pragma once

#include "renderer/KeyRenderer.h"
#include "renderer/TextRenderer.h"
#include "keyboard/KeyboardLayout.h"
#include "config/Config.h"
#include <string>
#include <utility>
#include <vector>

class Renderer
{
public:
  Renderer();
  ~Renderer();

  bool initialize(int framebufferWidth, int framebufferHeight);
  // hovered/pressed are key indices. fingertips and targets are in keyboard space [0,1].
  // injectArmed drives the INJECT ON/OFF status badge above the keyboard.
  void renderFrame(const KeyboardLayout &layout, const std::vector<int> &hovered,
                   const std::vector<int> &pressed, const Config &config,
                   const std::vector<std::pair<float, float>> &fingertipsKb,
                   const std::vector<std::pair<float, float>> &targetsKb,
                   bool injectArmed);

  // Mouse mode: hides the keyboard, shows a status badge and a live pose hint.
  void renderMouseMode(bool injectArmed, const std::string &poseHint);

private:
  std::pair<float, float> kbToNdc(float kx, float ky) const;
  KeyRenderer keyRenderer;
  TextRenderer textRenderer;
  NdcRegion region{-0.85f, -0.75f, 0.85f, 0.15f};
};
