#pragma once

#include "Renderer.h"

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
    
    void debugLog(const std::string&);
    void warningLog(const std::string&);
    void errorLog(const std::string&);
    void fatalLog(const std::string&);
}
