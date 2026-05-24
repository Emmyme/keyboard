#include <glad/glad.h>
#include "KeyRenderer.h"

KeyRenderer::KeyRenderer() = default;

KeyRenderer::~KeyRenderer()
{
  if (vbo) glDeleteBuffers(1, &vbo);
  if (vao) glDeleteVertexArrays(1, &vao);
}

bool KeyRenderer::initialize()
{
  if (!shader.loadFromFiles("assets/shaders/key.vert", "assets/shaders/key.frag"))
  {
    return false;
  }

  const float quad[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
  };
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);
  glBindVertexArray(0);
  return true;
}

void KeyRenderer::draw(const std::vector<KeyDefinition> &keys,
                       const std::vector<unsigned char> &states, const Config &config,
                       const NdcRegion &region)
{
  shader.use();
  glBindVertexArray(vao);
  const float w = region.xMax - region.xMin;
  const float h = region.yMax - region.yMin;

  for (size_t i = 0; i < keys.size(); ++i)
  {
    const auto &k = keys[i];
    float xMin = region.xMin + k.x * w;
    float xMax = region.xMin + (k.x + k.w) * w;
    float yTop = region.yMax - k.y * h;
    float yBot = region.yMax - (k.y + k.h) * h;

    unsigned char st = i < states.size() ? states[i] : 0;
    Color color = config.idleColor;
    float intensity = config.idleIntensity;
    if (st == 2) { color = config.pressColor; intensity = config.pressIntensity; }
    else if (st == 1) { color = config.hoverColor; intensity = config.hoverIntensity; }

    float aspect = (xMax - xMin) / (yTop - yBot);
    shader.setVec2("uRectMin", xMin, yBot);
    shader.setVec2("uRectMax", xMax, yTop);
    shader.setVec3("uColor", color[0], color[1], color[2]);
    shader.setFloat("uIntensity", intensity);
    shader.setFloat("uAspect", aspect);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  glBindVertexArray(0);
}

void KeyRenderer::drawDots(const std::vector<std::pair<float, float>> &pts,
                           float r, float g, float b, float s)
{
  shader.use();
  glBindVertexArray(vao);
  for (const auto &p : pts)
  {
    shader.setVec2("uRectMin", p.first - s, p.second - s);
    shader.setVec2("uRectMax", p.first + s, p.second + s);
    shader.setVec3("uColor", r, g, b);
    shader.setFloat("uIntensity", 1.4f);
    shader.setFloat("uAspect", 1.0f);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  glBindVertexArray(0);
}
