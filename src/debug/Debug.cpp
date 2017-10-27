#include "debug/Debug.h"

namespace lava
{
  namespace
  {
    HWND sWindowHandle = nullptr;
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

  void setWindowHandle(void* _hwnd)
  {
    sWindowHandle = (HWND)(_hwnd);
  }

  bool checkError(VkResult _result, const char* _text)
  {
    bool ignoreNextErrors = false;
    if (_result != VK_SUCCESS)
    {
      std::string msg;
#define VK_CASE_ERROR(x) case x: msg = #x; break;

      switch (_result)
      {
        VK_CASE_ERROR(VK_SUCCESS)
          VK_CASE_ERROR(VK_NOT_READY)
          VK_CASE_ERROR(VK_TIMEOUT)
          VK_CASE_ERROR(VK_EVENT_SET)
          VK_CASE_ERROR(VK_EVENT_RESET)
          VK_CASE_ERROR(VK_INCOMPLETE)
          VK_CASE_ERROR(VK_ERROR_OUT_OF_HOST_MEMORY)
          VK_CASE_ERROR(VK_ERROR_OUT_OF_DEVICE_MEMORY)
          VK_CASE_ERROR(VK_ERROR_INITIALIZATION_FAILED)
          VK_CASE_ERROR(VK_ERROR_DEVICE_LOST)
          VK_CASE_ERROR(VK_ERROR_MEMORY_MAP_FAILED)
          VK_CASE_ERROR(VK_ERROR_LAYER_NOT_PRESENT)
          VK_CASE_ERROR(VK_ERROR_EXTENSION_NOT_PRESENT)
          VK_CASE_ERROR(VK_ERROR_FEATURE_NOT_PRESENT)
          VK_CASE_ERROR(VK_ERROR_INCOMPATIBLE_DRIVER)
          VK_CASE_ERROR(VK_ERROR_TOO_MANY_OBJECTS)
          VK_CASE_ERROR(VK_ERROR_FORMAT_NOT_SUPPORTED)
          VK_CASE_ERROR(VK_ERROR_FRAGMENTED_POOL)
          VK_CASE_ERROR(VK_ERROR_SURFACE_LOST_KHR)
          VK_CASE_ERROR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
          VK_CASE_ERROR(VK_SUBOPTIMAL_KHR)
          VK_CASE_ERROR(VK_ERROR_OUT_OF_DATE_KHR)
          VK_CASE_ERROR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
          VK_CASE_ERROR(VK_ERROR_VALIDATION_FAILED_EXT)
          VK_CASE_ERROR(VK_ERROR_INVALID_SHADER_NV)
          VK_CASE_ERROR(VK_ERROR_OUT_OF_POOL_MEMORY_KHR)
          VK_CASE_ERROR(VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR)
          VK_CASE_ERROR(VK_RESULT_RANGE_SIZE)
      }

#undef VK_CASE_ERROR

      switch (MessageBox(sWindowHandle, _text, msg.c_str(), MB_ABORTRETRYIGNORE))
      {
      case IDABORT: //Abort
        DebugBreak();
        break;
      case IDIGNORE:
        ignoreNextErrors = true;
      }

      errorLog(msg + _text);
    }

    return ignoreNextErrors;
  }

  bool checkAssert(bool _condition, const char* _text)
  {
    bool ignoreNextAsserts = false;
    if (!_condition)
    {
      switch (MessageBox(sWindowHandle, "ASSERT", _text, MB_ABORTRETRYIGNORE))
      {
      case IDABORT: //Abort
        DebugBreak();
        break;
      case IDIGNORE:
        ignoreNextAsserts = true;
      }
    }

    errorLog(_text);
    return ignoreNextAsserts;
  }

  void setOutputMessageFunction(std::function<void(MessageSeverity, const std::string&)> _fnc)
  {
    sOutputMessageFunction = _fnc;
  }

  void debugLog(const std::string& _msg)
  {
    if (sOutputMessageFunction != nullptr)
      sOutputMessageFunction(MessageSeverity::eDebug, _msg);
    else
    {
      OutputDebugStringA(_msg.c_str());
      OutputDebugStringA("\n");
    }
  }

  void warningLog(const std::string& _msg)
  {
    if (sOutputMessageFunction != nullptr)
      sOutputMessageFunction(MessageSeverity::eWarning, _msg);
    else
    {
      OutputDebugStringA(_msg.c_str());
      OutputDebugStringA("\n");
    }
  }

  void errorLog(const std::string& _msg)
  {
    if (sOutputMessageFunction != nullptr)
      sOutputMessageFunction(MessageSeverity::eError, _msg);
    else
    {
      OutputDebugStringA(_msg.c_str());
      OutputDebugStringA("\n");
    }
  }

  void fatalLog(const std::string& _msg)
  {
    if (sOutputMessageFunction != nullptr)
      sOutputMessageFunction(MessageSeverity::eFatal, _msg);
    else
    {
      OutputDebugStringA(_msg.c_str());
      OutputDebugStringA("\n");
    }
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
