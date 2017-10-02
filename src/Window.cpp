#include "Window.h"

namespace
{
  LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
  {
    auto ptr = ::GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto window = reinterpret_cast<lava::Window*> (ptr);

    switch (uMsg)
    {
    case WM_CLOSE:
      window->Close();
      return 0;
    }

    return ::DefWindowProcA(hwnd, uMsg, wParam, lParam);
  }
}   // namespace

namespace lava
{
  Window::Window(const std::string& _title, const int _width, const int _height)
    : mWidth(_width)
    , mHeight(_height)
    , mTitle(_title)
  {
    DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

    ::RECT rect;
    ::SetRect(&rect, 0, 0, _width, _height);
    ::AdjustWindowRect(&rect, style, FALSE);

    ::WNDCLASSA wc;

    // Register the window class for the main window.
    wc.style = 0;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = NULL;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _title.c_str();

    ::RegisterClassA(&wc);

    // Create the main window.
    mHwnd = ::CreateWindowA(_title.c_str(),
      _title.c_str(),
      style, CW_USEDEFAULT, CW_USEDEFAULT,
      rect.right - rect.left, rect.bottom - rect.top, (HWND)NULL,
      (HMENU)NULL, NULL, (LPVOID)NULL);

    ::SetWindowLongPtr(mHwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR> (this));

    // Show the window and paint its contents.
    ::ShowWindow(mHwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(mHwnd);
  }

  Window::~Window()
  {
    ::UnregisterClassA(mTitle.c_str(), (HINSTANCE)::GetModuleHandle(NULL));
  }

  void Window::Update()
  {
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    ::UpdateWindow(mHwnd);
    while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  }
}
