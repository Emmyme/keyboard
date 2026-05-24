#include "KeyInjector.h"
#include <windows.h>
#include <utility>

namespace
{
// One synthetic key press: down then up for the given virtual-key code, wrapped
// in Shift down/up when `shift` is set (so the key produces its uppercase form).
bool sendInputKey(uint32_t virtualKey, bool shift)
{
  INPUT in[4] = {};
  int n = 0;
  if (shift) { in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = VK_SHIFT; ++n; }
  in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = static_cast<WORD>(virtualKey); ++n;
  in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = static_cast<WORD>(virtualKey);
  in[n].ki.dwFlags = KEYEVENTF_KEYUP; ++n;
  if (shift) { in[n].type = INPUT_KEYBOARD; in[n].ki.wVk = VK_SHIFT;
               in[n].ki.dwFlags = KEYEVENTF_KEYUP; ++n; }
  return SendInput(static_cast<UINT>(n), in, sizeof(INPUT)) == static_cast<UINT>(n);
}
} // namespace

KeyInjector::KeyInjector() : sender_(sendInputKey) {}
KeyInjector::KeyInjector(SendFn sender) : sender_(std::move(sender)) {}

bool KeyInjector::tap(uint32_t virtualKey, bool shift)
{
  if (!armed_ || virtualKey == 0 || !sender_) return false;
  return sender_(virtualKey, shift);
}
