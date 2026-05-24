#pragma once

#include <Windows.h>
#include <string>

class OverlayWindow
{
public:
  OverlayWindow();
  ~OverlayWindow();

  bool initialize(const std::string &title);
  bool processEvents();
  void swapBuffers();

  int framebufferWidth() const;
  int framebufferHeight() const;

private:
  HWND windowHandle = nullptr;
  HDC deviceContext = nullptr;
  HGLRC renderContext = nullptr;
  int width = 1280;
  int height = 720;
  static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
