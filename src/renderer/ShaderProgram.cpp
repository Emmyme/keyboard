#include "ShaderProgram.h"
#include "util/FileIO.h"
#include <iostream>

ShaderProgram::ShaderProgram() = default;

ShaderProgram::~ShaderProgram()
{
  if (programId)
  {
    glDeleteProgram(programId);
  }
}

GLuint ShaderProgram::compile(GLenum type, const std::string &source) const
{
  GLuint shader = glCreateShader(type);
  const char *src = source.c_str();
  glShaderSource(shader, 1, &src, nullptr);
  glCompileShader(shader);

  GLint ok = GL_FALSE;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
  if (!ok)
  {
    char log[1024];
    glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
    std::cerr << "Shader compile error: " << log << std::endl;
    glDeleteShader(shader);
    return 0;
  }
  return shader;
}

bool ShaderProgram::load(const std::string &vertexSource, const std::string &fragmentSource)
{
  GLuint vertexShader = compile(GL_VERTEX_SHADER, vertexSource);
  GLuint fragmentShader = compile(GL_FRAGMENT_SHADER, fragmentSource);
  if (!vertexShader || !fragmentShader)
  {
    // One may have compiled while the other failed, so don't leak the survivor.
    if (vertexShader)
    {
      glDeleteShader(vertexShader);
    }
    if (fragmentShader)
    {
      glDeleteShader(fragmentShader);
    }
    return false;
  }

  // Replacing an existing program (e.g. a reload), delete the old one first.
  if (programId)
  {
    glDeleteProgram(programId);
    programId = 0;
  }
  uniformCache.clear();

  programId = glCreateProgram();
  glAttachShader(programId, vertexShader);
  glAttachShader(programId, fragmentShader);
  glLinkProgram(programId);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  GLint ok = GL_FALSE;
  glGetProgramiv(programId, GL_LINK_STATUS, &ok);
  if (!ok)
  {
    char log[1024];
    glGetProgramInfoLog(programId, sizeof(log), nullptr, log);
    std::cerr << "Program link error: " << log << std::endl;
    glDeleteProgram(programId);
    programId = 0;
    return false;
  }
  return true;
}

bool ShaderProgram::loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath)
{
  auto vert = readTextFile(vertexPath);
  auto frag = readTextFile(fragmentPath);
  if (!vert || !frag)
  {
    std::cerr << "Shader file missing: " << vertexPath << " / " << fragmentPath << std::endl;
    return false;
  }
  return load(*vert, *frag);
}

void ShaderProgram::use() const
{
  if (programId)
  {
    glUseProgram(programId);
  }
}

GLint ShaderProgram::location(const std::string &name) const
{
  auto it = uniformCache.find(name);
  if (it != uniformCache.end())
  {
    return it->second;
  }
  GLint loc = glGetUniformLocation(programId, name.c_str());
  uniformCache.emplace(name, loc);
  return loc;
}

void ShaderProgram::setFloat(const std::string &name, float value) const
{
  glUniform1f(location(name), value);
}

void ShaderProgram::setVec2(const std::string &name, float x, float y) const
{
  glUniform2f(location(name), x, y);
}

void ShaderProgram::setVec3(const std::string &name, float x, float y, float z) const
{
  glUniform3f(location(name), x, y, z);
}
