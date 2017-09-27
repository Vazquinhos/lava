#include "Debug.h"

namespace lava
{
    namespace
    {
        std::function<void(MessageSeverity, const std::string&)> sOutputMessageFunction = nullptr;

        VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(
            VkDebugReportFlagsEXT       /*flags*/,
            VkDebugReportObjectTypeEXT  /*objectType*/,
            uint64_t                    /*object*/,
            size_t                      /*location*/,
            int32_t                     /*messageCode*/,
            const char*                 /*pLayerPrefix*/,
            const char*                 pMessage,
            void*                       /*pUserData*/)
        {
            OutputDebugStringA(pMessage);
            OutputDebugStringA("\n");
            return VK_FALSE;
        }
    }

    void setOutputMessageFunction(std::function<void(MessageSeverity, const std::string&)> _fnc )
    {
        sOutputMessageFunction = _fnc;
    }

    void debugLog(const std::string& _msg)
    {
        assert(sOutputMessageFunction != nullptr);
        sOutputMessageFunction(MessageSeverity::eDebug, _msg);
    }

    void warningLog(const std::string& _msg)
    {
        assert(sOutputMessageFunction != nullptr);
        sOutputMessageFunction(MessageSeverity::eWarning, _msg);
    }

    void errorLog(const std::string& _msg)
    {
        assert(sOutputMessageFunction != nullptr);
        sOutputMessageFunction(MessageSeverity::eError, _msg);
    }

    void fatalLog(const std::string& _msg)
    {
        assert(sOutputMessageFunction != nullptr);
        sOutputMessageFunction(MessageSeverity::eFatal, _msg);
    }

    Debug::Debug(VkInstance _instance)
        : mCallback(VK_NULL_HANDLE)
        , mInstance(_instance)
    {
#define GET_INSTANCE_ENTRYPOINT(i, w) w = reinterpret_cast<PFN_##w>(vkGetInstanceProcAddr(i, #w))
#define GET_DEVICE_ENTRYPOINT(i, w) w = reinterpret_cast<PFN_##w>(vkGetDeviceProcAddr(i, #w))

        GET_INSTANCE_ENTRYPOINT(mInstance, vkCreateDebugReportCallbackEXT);
        GET_INSTANCE_ENTRYPOINT(mInstance, vkDebugReportMessageEXT);
        GET_INSTANCE_ENTRYPOINT(mInstance, vkDestroyDebugReportCallbackEXT);

#undef GET_INSTANCE_ENTRYPOINT
#undef GET_DEVICE_ENTRYPOINT

        if (vkCreateDebugReportCallbackEXT)
        {
            VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
            callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
            callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            callbackCreateInfo.pfnCallback = &DebugReportCallback;
            vkCreateDebugReportCallbackEXT(mInstance, &callbackCreateInfo, nullptr, &mCallback);
        }
    }

    Debug::~Debug()
    {
        if (vkDestroyDebugReportCallbackEXT)
            vkDestroyDebugReportCallbackEXT(mInstance, mCallback, nullptr);
    }
}
