#pragma once

#include <string>
#include <optional>
#include <fstream>
#include <sstream>

// Reads an entire text file. Returns nullopt if the file cannot be opened.
inline std::optional<std::string> readTextFile(const std::string &path)
{
  std::ifstream file(path, std::ios::binary);
  if (!file)
  {
    return std::nullopt;
  }
  std::ostringstream ss;
  ss << file.rdbuf();
  return ss.str();
}
