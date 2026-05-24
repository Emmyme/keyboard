#pragma once

#include <cstdint>
#include <functional>

// Synthesizes real OS keystrokes, gated by an "armed" flag so the overlay can
// run in a safe preview-only mode by default. The actual send is a swappable
// seam, so the arming logic is unit-testable without firing real keystrokes.
class KeyInjector
{
public:
  using SendFn = std::function<bool(uint32_t /*virtualKey*/, bool /*shift*/)>;

  KeyInjector();                       // sends via the real Win32 SendInput
  explicit KeyInjector(SendFn sender); // for tests / custom backends

  void setArmed(bool armed) { armed_ = armed; }
  void toggle() { armed_ = !armed_; }
  bool armed() const { return armed_; }

  // Emits a key down+up for the Win32 virtual-key code, optionally wrapped in
  // Shift (for uppercase). No-op (returns false) when disarmed or the code is 0.
  // Returns true only if a key was actually sent.
  bool tap(uint32_t virtualKey, bool shift = false);

private:
  SendFn sender_;
  bool armed_ = false;
};
