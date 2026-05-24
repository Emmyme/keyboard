#pragma once

#include <glad/glad.h>
#include <string>
#include <unordered_map>

class ShaderProgram
{
public:
  ShaderProgram();
  ~ShaderProgram();

  bool load(const std::string &vertexSource, const std::string &fragmentSource);
  bool loadFromFiles(const std::string &vertexPath, const std::string &fragmentPath);
  void use() const;

  void setFloat(const std::string &name, float value) const;
  void setVec2(const std::string &name, float x, float y) const;
  void setVec3(const std::string &name, float x, float y, float z) const;

private:
  GLuint compile(GLenum type, const std::string &source) const;
  GLint location(const std::string &name) const; // cached uniform lookup

  GLuint programId = 0;
  mutable std::unordered_map<std::string, GLint> uniformCache;
};
