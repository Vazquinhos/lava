#include "Debug.h"

namespace lava
{
    namespace
    {
        std::function<void(MessageSeverity, const std::string&)> sOutputMessageFunction = nullptr;
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
}
