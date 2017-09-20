#include "Renderer.h"
#include "Debug.h"

namespace lava
{
    Renderer::Renderer(HWND _hwnd, const std::vector< std::string >& _requiredExtensions, const std::vector< std::string >& _requiredLayers)
        : mhwnd(_hwnd)
    {
        // The extensions asked by the user must be checked with the available extensions in this platform
        uint32_t extensionCount = 0;
        VkResult r = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
        assert(r == VK_SUCCESS);

        std::vector<VkExtensionProperties> instanceExtensions{ extensionCount };
        r = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instanceExtensions.data());
        assert(r == VK_SUCCESS);

        // The same goes for the layers asked by the user...
        uint32_t layerCount = 0;
        r = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        assert(r == VK_SUCCESS);

        std::vector<VkLayerProperties> instanceLayers{ layerCount };
        r = vkEnumerateInstanceLayerProperties(&layerCount, instanceLayers.data());
        assert(r == VK_SUCCESS);

        std::vector<const char*> extensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
        std::vector<const char*> layers;

#ifdef  _DEBUG
        const size_t numExtensions = _requiredExtensions.size();
        debugLog("Extensions:");
        for (const auto& e : instanceExtensions)
        {
            if (strcmp(e.extensionName, "VK_EXT_debug_report") == 0)
            {
                extensions.push_back("VK_EXT_debug_report");
                debugLog("VK_EXT_debug_report");
                continue;
            }

            bool extensionFound = false;
            for (size_t iExtension = 0; iExtension < numExtensions && extensionFound == false; ++iExtension)
            {
                if (strcmp(e.extensionName, _requiredExtensions[iExtension].c_str()) == 0)
                {
                    extensionFound = true;
                    extensions.push_back(e.extensionName);
                    debugLog(e.extensionName);
                }
            }
        }

        const size_t numLayers = _requiredLayers.size();
        debugLog("Layers");
        for (const auto& l : instanceLayers)
        {
            if (strcmp(l.layerName, "VK_LAYER_LUNARG_standard_validation") == 0)
            {
                layers.push_back("VK_LAYER_LUNARG_standard_validation");
                debugLog("VK_LAYER_LUNARG_standard_validation");
                continue;
            }

            bool layerFound = false;
            for (size_t iLayer = 0; iLayer < numLayers && layerFound == false; ++iLayer)
            {
                if (strcmp(l.layerName, _requiredLayers[iLayer].c_str()) == 0)
                {
                    layerFound = true;
                    layers.push_back(l.layerName);
                    debugLog(l.layerName);
                }
            }
        }
#else
        if (!_requiredExtensions.empty())
        {
            const size_t numExtensions = _requiredExtensions.size();
            for (const auto& e : instanceExtensions)
            {
                bool extensionFound = false;
                for (size_t iExtension = 0; iExtension < numExtensions && extensionFound == false; ++iExtension)
                {
                    if (strcmp(e.extensionName, _requiredExtensions[iExtension].c_str()) == 0)
                    {
                        extensionFound = true;
                        extensions.push_back(e.extensionName);
                    }
                }
            }
        }

        if (!_requiredLayers.empty())
        {
            const size_t numLayers = _requiredLayers.size();
            for (const auto& l : instanceLayers)
            {
                bool layerFound = false;
                for (size_t iLayer = 0; iLayer < numLayers && layerFound == false; ++iLayer)
                {
                    if (strcmp(l.layerName, _requiredLayers[iLayer].c_str()) == 0)
                    {
                        layerFound = true;
                        layers.push_back(l.layerName);
                    }
                }
            }
        }
#endif
        CreateInstance(extensions, layers);
        CreateDeviceAndQueue();
        CreateSurface();
    }

    Renderer::~Renderer()
    {
        vkDestroyDevice(mDevice, nullptr);
        vkDestroyInstance(mInstance, nullptr);
    }

    bool Renderer::Update()
    {
        return false;
    }

    void Renderer::CreateInstance(const std::vector<const char*>& _requiredAvailableExtensions, const std::vector< const char* >& _requiredAvailableLayers)
    {
        VkInstanceCreateInfo instanceCreateInfo = {};
        instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceCreateInfo.ppEnabledExtensionNames = _requiredAvailableExtensions.data();
        instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t> (_requiredAvailableExtensions.size());
        instanceCreateInfo.ppEnabledLayerNames = _requiredAvailableLayers.data();
        instanceCreateInfo.enabledLayerCount = static_cast<uint32_t> (_requiredAvailableLayers.size());

        VkApplicationInfo applicationInfo = {};
        applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        applicationInfo.apiVersion = VK_API_VERSION_1_0;
        applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        applicationInfo.pApplicationName = "lava engine app";
        applicationInfo.pEngineName = "lava engine";

        instanceCreateInfo.pApplicationInfo = &applicationInfo;

        VkResult r = vkCreateInstance(&instanceCreateInfo, nullptr, &mInstance);
        assert(r == VK_SUCCESS);
    }

    void Renderer::CreateDeviceAndQueue()
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, nullptr);

        std::vector<VkPhysicalDevice> devices{ physicalDeviceCount };
        vkEnumeratePhysicalDevices(mInstance, &physicalDeviceCount, devices.data());

        int graphicsQueueIndex = -1;
        for (auto physicalDevice : devices)
        {
            uint32_t queueFamilyPropertyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilyProperties{ queueFamilyPropertyCount };
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyPropertyCount, queueFamilyProperties.data());

            for (size_t iQueue = 0, count = queueFamilyProperties.size(); iQueue < count && mPhysicalDevice == VK_NULL_HANDLE; ++iQueue)
            {
                if (queueFamilyProperties[iQueue].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                {
                    mPhysicalDevice = physicalDevice;
                    graphicsQueueIndex = static_cast< uint32_t >( iQueue );
                }
            }
        }

        assert(mPhysicalDevice);
        assert(graphicsQueueIndex > -1);

        VkDeviceQueueCreateInfo deviceQueueCreateInfo = {};
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.queueCount = 1;
        deviceQueueCreateInfo.queueFamilyIndex = graphicsQueueIndex;

        static const float queuePriorities[] = { 1.0f };
        deviceQueueCreateInfo.pQueuePriorities = queuePriorities;

        VkDeviceCreateInfo deviceCreateInfo = {};
        deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;

        std::vector<const char*> physicalDeviceLayers;

        #ifdef _DEBUG
        uint32_t layerCount = 0;
        vkEnumerateDeviceLayerProperties(mPhysicalDevice, &layerCount, nullptr);

        std::vector<VkLayerProperties> deviceLayers{ layerCount };
        vkEnumerateDeviceLayerProperties(mPhysicalDevice, &layerCount, deviceLayers.data());

        std::vector<const char*> result;
        for (const auto& p : deviceLayers)
        {
            if (strcmp(p.layerName, "VK_LAYER_LUNARG_standard_validation") == 0)
                physicalDeviceLayers.push_back("VK_LAYER_LUNARG_standard_validation");
        }
        #endif

        deviceCreateInfo.ppEnabledLayerNames = physicalDeviceLayers.data();
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t> (physicalDeviceLayers.size());

        std::vector<const char*> deviceExtensions = { "VK_KHR_swapchain" };

        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t> (deviceExtensions.size());

        vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice);
        assert(mDevice);

        vkGetDeviceQueue(mDevice, graphicsQueueIndex, 0, &mQueue);
        assert(mQueue);
    }

    void Renderer::CreateSurface()
    {
        VkWin32SurfaceCreateInfoKHR win32surfaceCreateInfo = {};
        win32surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        win32surfaceCreateInfo.hwnd = mhwnd;
        win32surfaceCreateInfo.hinstance = ::GetModuleHandle(nullptr);
        vkCreateWin32SurfaceKHR(mInstance, &win32surfaceCreateInfo, nullptr, &mSurface);
        assert(mSurface);

        VkBool32 presentSupported;
        vkGetPhysicalDeviceSurfaceSupportKHR(mPhysicalDevice, 0, mSurface, &presentSupported);
        assert(presentSupported);
    }
}
