#include <glad/glad.h>
#include "Renderer.h"

Renderer::Renderer() = default;
Renderer::~Renderer() = default;

bool Renderer::initialize(int framebufferWidth, int framebufferHeight)
{
  glViewport(0, 0, framebufferWidth, framebufferHeight);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
  return keyRenderer.initialize() && textRenderer.initialize();
}

std::pair<float, float> Renderer::kbToNdc(float kx, float ky) const
{
  float w = region.xMax - region.xMin;
  float h = region.yMax - region.yMin;
  return {region.xMin + kx * w, region.yMax - ky * h}; // y down -> NDC y up
}

void Renderer::renderFrame(const KeyboardLayout &layout, const std::vector<int> &hovered,
                           const std::vector<int> &pressed, const Config &config,
                           const std::vector<std::pair<float, float>> &fingertipsKb,
                           const std::vector<std::pair<float, float>> &targetsKb,
                           bool injectArmed)
{
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  std::vector<unsigned char> states(layout.keys().size(), 0);
  for (int i : hovered)
    if (i >= 0 && i < static_cast<int>(states.size())) states[i] = 1;
  for (int i : pressed)
    if (i >= 0 && i < static_cast<int>(states.size())) states[i] = 2;

  keyRenderer.draw(layout.keys(), states, config, region);

  // Glowing laser labels centered on each key (shrunk to fit wide keys).
  const float regH = region.yMax - region.yMin;
  const float regW = region.xMax - region.xMin;
  for (const auto &k : layout.keys())
  {
    auto c = kbToNdc(k.x + k.w * 0.5f, k.y + k.h * 0.5f);
    float labelH = k.h * regH * 0.42f;
    float labelMaxW = k.w * regW * 0.82f;
    textRenderer.drawCenteredFit(k.label, c.first, c.second, labelH, labelMaxW, 0.55f, 1.0f, 1.0f);
  }

  // Injection status badge above the keyboard: green when armed, dim red when off.
  if (injectArmed)
    textRenderer.drawCentered("INJECT ON", 0.0f, region.yMax + 0.10f, 0.05f, 0.3f, 1.0f, 0.45f);
  else
    textRenderer.drawCentered("INJECT OFF", 0.0f, region.yMax + 0.10f, 0.05f, 1.0f, 0.45f, 0.3f);

  // Fingertip dots (cyan) and calibration targets (magenta).
  std::vector<std::pair<float, float>> fdots, tdots;
  for (auto &f : fingertipsKb) fdots.push_back(kbToNdc(f.first, f.second));
  for (auto &t : targetsKb) tdots.push_back(kbToNdc(t.first, t.second));
  keyRenderer.drawDots(fdots, 0.2f, 1.0f, 1.0f, 0.02f);
  keyRenderer.drawDots(tdots, 1.0f, 0.3f, 1.0f, 0.03f);
}
