#include <glad/glad.h>
#include "TextRenderer.h"
#include "stb/stb_easy_font.h"
#include <vector>

TextRenderer::TextRenderer() = default;

TextRenderer::~TextRenderer()
{
  if (vbo_) glDeleteBuffers(1, &vbo_);
  if (vao_) glDeleteVertexArrays(1, &vao_);
}

bool TextRenderer::initialize()
{
  if (!shader_.loadFromFiles("assets/shaders/text.vert", "assets/shaders/text.frag"))
  {
    return false;
  }
  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  return true;
}

void TextRenderer::drawPass(const std::string &text, float cx, float cy, float ndcHeight,
                            float alpha)
{
  static char quads[60000];
  int n = stb_easy_font_print(0.0f, 0.0f, const_cast<char *>(text.c_str()), nullptr, quads,
                              sizeof(quads));
  if (n <= 0) return;

  const float textW = static_cast<float>(stb_easy_font_width(const_cast<char *>(text.c_str())));
  const float textH = 12.0f; // stb single-line cell height
  const float scale = ndcHeight / textH;
  const float *fb = reinterpret_cast<const float *>(quads); // 4 floats per vertex (x,y,z,color)

  std::vector<float> verts;
  verts.reserve(static_cast<size_t>(n) * 12);
  auto place = [&](int vi, float &ox, float &oy) {
    float x = fb[vi * 4 + 0], y = fb[vi * 4 + 1];
    ox = cx + (x - textW * 0.5f) * scale;
    oy = cy - (y - textH * 0.5f) * scale; // flip y (stb y is downward)
  };
  for (int q = 0; q < n; ++q)
  {
    float p[4][2];
    for (int k = 0; k < 4; ++k) place(q * 4 + k, p[k][0], p[k][1]);
    const int idx[6] = {0, 1, 2, 0, 2, 3};
    for (int t : idx) { verts.push_back(p[t][0]); verts.push_back(p[t][1]); }
  }

  shader_.setFloat("uAlpha", alpha);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_DYNAMIC_DRAW);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(verts.size() / 2));
  glBindVertexArray(0);
}

void TextRenderer::drawCentered(const std::string &text, float cx, float cy, float ndcHeight,
                                float r, float g, float b)
{
  if (text.empty()) return;
  shader_.use();
  shader_.setVec3("uColor", r, g, b);
  drawPass(text, cx, cy, ndcHeight * 1.35f, 0.30f); // soft halo
  drawPass(text, cx, cy, ndcHeight, 1.0f);          // bright core
}

void TextRenderer::drawCenteredFit(const std::string &text, float cx, float cy, float maxHeight,
                                   float maxWidth, float r, float g, float b)
{
  if (text.empty()) return;
  const float w = static_cast<float>(stb_easy_font_width(const_cast<char *>(text.c_str())));
  const float textH = 12.0f; // stb cell height. rendered width = w * (height/textH)
  float height = maxHeight;
  if (w > 0.0f)
  {
    const float heightForWidth = maxWidth * textH / w;
    if (heightForWidth < height) height = heightForWidth;
  }
  drawCentered(text, cx, cy, height, r, g, b);
}
