#include "Config.h"
#include "util/FileIO.h"
#include <nlohmann/json.hpp>

void Config::loadDefault()
{
  *this = Config{}; // member initializers above are the defaults
}

namespace
{
void readColor(const nlohmann::json &j, const char *key, Color &out)
{
  if (j.contains(key) && j[key].is_array() && j[key].size() == 3)
  {
    out = {j[key][0].get<float>(), j[key][1].get<float>(), j[key][2].get<float>()};
  }
}
void readFloat(const nlohmann::json &j, const char *key, float &out)
{
  if (j.contains(key) && j[key].is_number())
  {
    out = j[key].get<float>();
  }
}
}

bool Config::loadFromFile(const std::string &path)
{
  auto text = readTextFile(path);
  if (!text)
  {
    return false;
  }
  nlohmann::json j = nlohmann::json::parse(*text, nullptr, false);
  if (j.is_discarded())
  {
    return false; // malformed JSON
  }
  readColor(j, "idleColor", idleColor);
  readColor(j, "hoverColor", hoverColor);
  readColor(j, "pressColor", pressColor);
  readFloat(j, "idleIntensity", idleIntensity);
  readFloat(j, "hoverIntensity", hoverIntensity);
  readFloat(j, "pressIntensity", pressIntensity);
  return true;
}
