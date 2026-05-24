#include <glad/glad.h>   // must precede any GL/Windows GL include
#include "OverlayWindow.h"
#include <dwmapi.h>

OverlayWindow::OverlayWindow() = default;

OverlayWindow::~OverlayWindow()
{
  if (renderContext)
  {
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(renderContext);
  }
  if (deviceContext)
  {
    ReleaseDC(windowHandle, deviceContext);
  }
  if (windowHandle)
  {
    DestroyWindow(windowHandle);
  }
}

bool OverlayWindow::initialize(const std::string &title)
{
  HINSTANCE instance = GetModuleHandle(nullptr);

  WNDCLASSA wc = {};
  wc.lpfnWndProc = WindowProc;
  wc.hInstance = instance;
  wc.lpszClassName = "LaserKeyboardOverlay";
  RegisterClassA(&wc);

  // No WS_EX_LAYERED: a layered top-level window that never sets layer attributes
  // is invisible, and it conflicts with the DWM per-pixel-alpha path below.
  // WS_EX_TRANSPARENT keeps the overlay click-through. WS_EX_NOACTIVATE keeps it
  // from stealing focus, so injected keystrokes land in the user's target app.
  windowHandle = CreateWindowExA(
      WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
      wc.lpszClassName,
      title.c_str(),
      WS_POPUP,
      0, 0, width, height,
      nullptr, nullptr, instance, nullptr);
  if (!windowHandle)
  {
    return false;
  }

  deviceContext = GetDC(windowHandle);
  if (!deviceContext)
  {
    return false;
  }

  // Pixel format WITH an alpha channel, required for per-pixel transparency.
  PIXELFORMATDESCRIPTOR pfd = {};
  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 32;
  pfd.cAlphaBits = 8;
  pfd.cDepthBits = 24;

  int pixelFormat = ChoosePixelFormat(deviceContext, &pfd);
  if (!pixelFormat || !SetPixelFormat(deviceContext, pixelFormat, &pfd))
  {
    return false;
  }

  renderContext = wglCreateContext(deviceContext);
  if (!renderContext || !wglMakeCurrent(deviceContext, renderContext))
  {
    return false;
  }

  if (!gladLoadGL())
  {
    return false;
  }

  // Enable DWM per-pixel alpha across the whole window. An empty blur region
  // (right/bottom < left/top) turns on alpha compositing WITHOUT a frosted blur,
  // so cleared-to-(0,0,0,0) areas are fully transparent and only the glow shows.
  DWM_BLURBEHIND blurBehind = {};
  HRGN region = CreateRectRgn(0, 0, -1, -1);
  blurBehind.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
  blurBehind.fEnable = TRUE;
  blurBehind.hRgnBlur = region;
  DwmEnableBlurBehindWindow(windowHandle, &blurBehind);
  DeleteObject(region);

  ShowWindow(windowHandle, SW_SHOWNOACTIVATE); // show without taking focus
  return true;
}

bool OverlayWindow::processEvents()
{
  MSG msg;
  while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
    {
      return false;
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
  return true;
}

void OverlayWindow::swapBuffers()
{
  SwapBuffers(deviceContext);
}

int OverlayWindow::framebufferWidth() const
{
  return width;
}

int OverlayWindow::framebufferHeight() const
{
  return height;
}

LRESULT CALLBACK OverlayWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  default:
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}
