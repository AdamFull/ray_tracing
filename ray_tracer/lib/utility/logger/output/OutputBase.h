#pragma once

#include <string>

namespace utl
{
    enum class EOutputType
    {
        eCOUT,
        eFile,
        eWinCmd
    };

    enum class ELogLevel
    {
        eDebug,
        eVerbose,
        eError,
        eWarning,
        eInfo
    };

    class COutputBase
    {
    public:
        virtual ~COutputBase() = default;
        virtual void init(const std::string& app_name, const std::string& app_version);
        virtual void log(const std::string& message, ELogLevel eLevel) = 0;

    protected:
        virtual void write(const std::string& write) = 0;
    };
}