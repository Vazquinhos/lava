#pragma once

#include <string>
#include <Windows.h>
#include <memory>

namespace lava
{
    class Window
    {
    public:
        Window(const std::string& _title, const int _width, const int _height);
        virtual ~Window();

        HWND GetHWND() const
        {
            return mHwnd;
        }

        bool IsClosed() const
        {
            return mIsClosed;
        }

        void Close()
        {
            mIsClosed = true;
        }

        int GetWidth() const
        {
            return mWidth;
        }

        int GetHeight() const
        {
            return mHeight;
        }

        void Update()
        {
            ::UpdateWindow(mHwnd);
        }

    protected:
        HWND mHwnd = 0;
        bool mIsClosed = false;
        int  mWidth = -1, mHeight = -1;
        const std::string mTitle = "LavaWindow";

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
    };
}
