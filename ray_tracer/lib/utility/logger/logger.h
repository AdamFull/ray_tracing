#pragma once

#include <memory>
#include <vector>
#include "formatter.h"

#include "output/OutputCOUT.h"
#include "output/OutputFile.h"
#include "output/OutputWinCmd.h"

#include "backward-cpp/backward.hpp"
#include "debugbreak.h"

namespace utl
{
    class CLogger
    {
    protected:
        CLogger() = default;
    public:
        friend std::unique_ptr<CLogger> std::make_unique<CLogger>();

        static const std::unique_ptr<CLogger>& getInstance()
        {
            static std::unique_ptr<CLogger> instance{nullptr};
            if(!instance)
                instance = std::make_unique<CLogger>();
            return instance;
        }

        void init(const std::string& app_name, const std::string& app_version);

        void addOutput(std::shared_ptr<COutputBase>&& output);

        template<ELogLevel _level = ELogLevel::eDebug, class ..._Args>
        void log(std::source_location&& loc, const std::string& trace, const std::string_view fmt, _Args&&... args)
        {
            auto formatted = formatter::format<_level>(std::move(loc), trace, fmt, std::forward<_Args>(args)...);

            for(auto& output : vOutputs)
                output->log(formatted, _level);
        }

    private:
        std::vector<std::shared_ptr<COutputBase>> vOutputs;
    };
}

#define log_add_file_output(filename) utl::CLogger::getInstance()->addOutput(std::make_shared<utl::COutputFile>(filename))
#define log_add_cout_output() utl::CLogger::getInstance()->addOutput(std::make_shared<utl::COutputCOUT>())
#define log_add_wincmd_output() utl::CLogger::getInstance()->addOutput(std::make_shared<utl::COutputWinCmd>())

#define log_init(app_name, app_version) utl::CLogger::getInstance()->init(app_name, app_version);

#define log_error(fmt, ...) \
{ \
backward::StackTrace st; \
st.load_here(32); \
backward::Printer p; \
p.object = true; \
p.color_mode = backward::ColorMode::always; \
p.address = true; \
std::stringstream ss; \
ss << "\n"; \
p.print(st, ss); \
utl::CLogger::getInstance()->log<utl::ELogLevel::eError>(std::source_location::current(), ss.str(), fmt, __VA_ARGS__); \
debugbreak(); \
}
#define log_cerror(cond, fmt, ...) if((!cond)) log_error(fmt, __VA_ARGS__)
#define log_warning(fmt, ...) utl::CLogger::getInstance()->log<utl::ELogLevel::eWarning>(std::source_location::current(), "", fmt, __VA_ARGS__)
#define log_info(fmt, ...) utl::CLogger::getInstance()->log<utl::ELogLevel::eInfo>(std::source_location::current(), "", fmt, __VA_ARGS__)

#ifdef NDEBUG
#define log_verbose(fmt, ...)
#define log_debug(fmt, ...)
#else
#define log_verbose(fmt, ...) utl::CLogger::getInstance()->log<utl::ELogLevel::eVerbose>(std::source_location::current(), "", fmt, __VA_ARGS__)
#define log_debug(fmt, ...) utl::CLogger::getInstance()->log<utl::ELogLevel::eDebug>(std::source_location::current(), "", fmt, __VA_ARGS__)
#endif