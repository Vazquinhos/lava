#pragma once
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <Windows.h>

namespace lava
{
    static const int sQueueSlotCount = 3;
    
    template<typename T>
    void safe_delete(T*& a)
    {
        delete a;
        a = nullptr;
    }
}
