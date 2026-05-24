#pragma once

#include "renderer/ShaderProgram.h"
#include <string>

// Draws glowing laser-style text using stb_easy_font geometry: a dim oversized
// halo pass behind a bright core pass.
class TextRenderer
{
public:
  TextRenderer();
  ~TextRenderer();

  bool initialize();
  // Centered at NDC (cx,cy). Glyph height ~ndcHeight, glow color (r,g,b).
  void drawCentered(const std::string &text, float cx, float cy, float ndcHeight,
                    float r, float g, float b);
  // Like drawCentered, but shrinks the glyph height if needed so the text also
  // fits within maxWidth (keeps wide labels like CAPS/SPACE inside their key).
  void drawCenteredFit(const std::string &text, float cx, float cy, float maxHeight,
                       float maxWidth, float r, float g, float b);

private:
  void drawPass(const std::string &text, float cx, float cy, float ndcHeight, float alpha);

  ShaderProgram shader_;
  unsigned int vao_ = 0;
  unsigned int vbo_ = 0;
};
