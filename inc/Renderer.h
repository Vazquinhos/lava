#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan.h>

#include <vector>
#include <memory>
#include <functional>

namespace lava
{
    class Renderer
    {
    public:
        Renderer(HWND _hwnd, const std::vector< std::string >& _requiredExtensions, const std::vector< std::string >& _requiredLayers);
        virtual ~Renderer();

        bool Update();
    
    protected: 
        VkInstance       mInstance          = VK_NULL_HANDLE;
        VkPhysicalDevice mPhysicalDevice    = VK_NULL_HANDLE;
        VkDevice         mDevice            = VK_NULL_HANDLE;
        VkQueue          mQueue             = VK_NULL_HANDLE;
        VkSurfaceKHR     mSurface           = VK_NULL_HANDLE;
        int              mQueueFamilyIndex  = -1;
        HWND             mhwnd              = nullptr;
    
    protected:
        void CreateInstance(const std::vector<const char*>& _requiredAvailableExtensions, const std::vector< const char* >& _requiredAvailableLayers);
        void CreateDeviceAndQueue();
        void CreateSurface();
    };
}
