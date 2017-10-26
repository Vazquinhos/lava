#include "render/Window.h"

#include "imgui/imgui_impl.h"

namespace
{
  static bool IsAnyMouseButtonDown()
  {
    ImGuiIO& io = ImGui::GetIO();
    for (int n = 0; n < ARRAYSIZE(io.MouseDown); n++)
      if (io.MouseDown[n])
        return true;
    return false;
  }

  LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
  {
    auto ptr = ::GetWindowLongPtr(hwnd, GWLP_USERDATA);
    auto window = reinterpret_cast<lava::Window*> (ptr);

    ImGuiIO& io = ImGui::GetIO();
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_MBUTTONDOWN:
    {
      int button = 0;
      if (msg == WM_LBUTTONDOWN) button = 0;
      if (msg == WM_RBUTTONDOWN) button = 1;
      if (msg == WM_MBUTTONDOWN) button = 2;
      if (!IsAnyMouseButtonDown() && GetCapture() == NULL)
        SetCapture(hwnd);
      io.MouseDown[button] = true;
      return 0;
    }
    case WM_LBUTTONUP:
    case WM_RBUTTONUP:
    case WM_MBUTTONUP:
    {
      int button = 0;
      if (msg == WM_LBUTTONUP) button = 0;
      if (msg == WM_RBUTTONUP) button = 1;
      if (msg == WM_MBUTTONUP) button = 2;
      io.MouseDown[button] = false;
      if (!IsAnyMouseButtonDown() && GetCapture() == hwnd)
        ReleaseCapture();
      return 0;
    }
    case WM_MOUSEWHEEL:
      io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
      return 0;
    case WM_MOUSEMOVE:
      io.MousePos.x = (signed short)(lParam);
      io.MousePos.y = (signed short)(lParam >> 16);
      return 0;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
      if (wParam < 256)
        io.KeysDown[wParam] = 1;
      return 0;
    case WM_KEYUP:
    case WM_SYSKEYUP:
      if (wParam < 256)
        io.KeysDown[wParam] = 0;
      return 0;
    case WM_CHAR:
      // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
      if (wParam > 0 && wParam < 0x10000)
        io.AddInputCharacter((unsigned short)wParam);
      return 0;
    case WM_CLOSE:
      window->Close();
      return 0;
    }
    return ::DefWindowProcA(hwnd, msg, wParam, lParam);
  }
}   // namespace

namespace lava
{
  Window::Window(const std::string& _title, const uint32_t _width, const uint32_t _height)
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
