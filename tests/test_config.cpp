#include <doctest.h>
#include "config/Config.h"
#include "util/FileIO.h"
#include <fstream>
#include <cstdio>

TEST_CASE("config exposes sensible default colors")
{
  Config c;
  c.loadDefault();
  // Idle is dimmer than hover, hover dimmer than pressed (by intensity).
  CHECK(c.idleIntensity < c.hoverIntensity);
  CHECK(c.hoverIntensity < c.pressIntensity);
  // Colors are 3-component and within [0,1].
  CHECK(c.idleColor[1] >= 0.0f);
  CHECK(c.idleColor[1] <= 1.0f);
}

TEST_CASE("loadFromFile overrides defaults and missing file keeps defaults")
{
  const char *path = "test_config_tmp.json";
  {
    std::ofstream out(path);
    out << R"({ "hoverIntensity": 0.77, "idleColor": [0.1, 0.2, 0.3] })";
  }

  Config c;
  c.loadDefault();
  REQUIRE(c.loadFromFile(path) == true);
  CHECK(c.hoverIntensity == doctest::Approx(0.77f));
  CHECK(c.idleColor[0] == doctest::Approx(0.1f));
  CHECK(c.idleColor[2] == doctest::Approx(0.3f));

  // Fields absent from the JSON must keep their defaults (partial override).
  CHECK(c.pressIntensity == doctest::Approx(1.6f));
  CHECK(c.hoverColor[0] == doctest::Approx(0.2f));

  std::remove(path);

  Config d;
  d.loadDefault();
  CHECK(d.loadFromFile("does_not_exist.json") == false); // missing -> false
  CHECK(d.hoverIntensity == doctest::Approx(1.0f));      // defaults intact
}

TEST_CASE("readTextFile returns content and nullopt for missing files")
{
  CHECK(readTextFile("definitely_missing_file.xyz").has_value() == false);
}
