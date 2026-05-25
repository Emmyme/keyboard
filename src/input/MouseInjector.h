#pragma once

#include <functional>

enum class MouseAction { Move, LeftDown, LeftUp };

struct MouseEvent
{
  MouseAction action;
  int dx = 0;
  int dy = 0;
};

// Clamps cur + delta into the [0,w) x [0,h) screen rect. Pure, for testing.
void clampToScreen(int curX, int curY, int dx, int dy, int w, int h, int &outX, int &outY);

// Turns mouse intents into real OS events behind an armed gate. The actual send
// is a swappable seam so the logic is testable without moving the real cursor.
class MouseInjector
{
public:
  using SendFn = std::function<bool(const MouseEvent &)>;

  MouseInjector();                       // real SetCursorPos / SendInput backend
  explicit MouseInjector(SendFn sender); // for tests

  void setArmed(bool a) { armed_ = a; }
  bool armed() const { return armed_; }

  bool moveBy(int dx, int dy);
  bool leftDown();
  bool leftUp();

private:
  SendFn sender_;
  bool armed_ = false;
};
