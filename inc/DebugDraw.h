#pragma once

#include "lava.h"

namespace lava
{
  enum class MessageSeverity
  {
    eDebug = 0,
    eWarning,
    eError,
    eFatal
  };

  VULKAN_HPP_INLINE std::string to_string(MessageSeverity value)
  {
    switch (value)
    {
    case MessageSeverity::eDebug: return "Debug";
    case MessageSeverity::eWarning: return "Warning";
    case MessageSeverity::eError: return "Error";
    case MessageSeverity::eFatal: return "Fatal";
    default: return "Unknown";
    }
  }

  void setOutputMessageFunction(std::function<void(MessageSeverity, const std::string&)>);
  void setWindowHandle(void* _hwnd);
  bool checkAssert(bool _condition, const char* _text);
  bool checkError(VkResult _result, const char* _text);

  void debugLog(const std::string&);
  void warningLog(const std::string&);
  void errorLog(const std::string&);
  void fatalLog(const std::string&);

  class Debug
  {
  public:
    Debug(VkInstance _instance);
    virtual ~Debug();
  protected:
    VkDebugReportCallbackEXT mCallback;
    VkInstance mInstance;
    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = nullptr;
    PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = nullptr;
    PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = nullptr;
  };
}
