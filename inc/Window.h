#pragma once

#include "lava.h"

namespace lava
{
    class Window
    {
    public:
        Window(const std::string& _title, const uint32_t _width, const uint32_t _height);
        virtual ~Window();

        HWND GetHWND() const
        {
            return mHwnd;
        }

        VkExtent2D size() const { return VkExtent2D({ mWidth, mHeight }); };

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

        void Update();

    protected:
        HWND mHwnd = nullptr;
        bool mIsClosed = false;
        uint32_t  mWidth = 800, mHeight = 600;
        const std::string mTitle = "LavaWindow";

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
    };
}
