#include "MouseInjector.h"
#include <windows.h>
#include <utility>

void clampToScreen(int curX, int curY, int dx, int dy, int w, int h, int &outX, int &outY)
{
  int nx = curX + dx;
  int ny = curY + dy;
  if (nx < 0) nx = 0;
  if (nx > w - 1) nx = w - 1;
  if (ny < 0) ny = 0;
  if (ny > h - 1) ny = h - 1;
  outX = nx;
  outY = ny;
}

namespace
{
bool realSend(const MouseEvent &e)
{
  INPUT in{};
  in.type = INPUT_MOUSE;
  switch (e.action)
  {
  case MouseAction::Move:
  {
    POINT p;
    if (!GetCursorPos(&p)) return false;
    int w = GetSystemMetrics(SM_CXSCREEN);
    int h = GetSystemMetrics(SM_CYSCREEN);
    int nx, ny;
    clampToScreen(p.x, p.y, e.dx, e.dy, w, h, nx, ny);
    return SetCursorPos(nx, ny) != 0;
  }
  case MouseAction::LeftDown: in.mi.dwFlags = MOUSEEVENTF_LEFTDOWN; break;
  case MouseAction::LeftUp: in.mi.dwFlags = MOUSEEVENTF_LEFTUP; break;
  }
  return SendInput(1, &in, sizeof(INPUT)) == 1;
}
} // namespace

MouseInjector::MouseInjector() : sender_(realSend) {}
MouseInjector::MouseInjector(SendFn sender) : sender_(std::move(sender)) {}

bool MouseInjector::moveBy(int dx, int dy)
{
  if (!armed_ || !sender_ || (dx == 0 && dy == 0)) return false;
  return sender_({MouseAction::Move, dx, dy});
}
bool MouseInjector::leftDown()
{
  if (!armed_ || !sender_) return false;
  return sender_({MouseAction::LeftDown});
}
bool MouseInjector::leftUp()
{
  if (!armed_ || !sender_) return false;
  return sender_({MouseAction::LeftUp});
}
