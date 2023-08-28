#pragma once

#include "OutputBase.h"

namespace utl
{
    class COutputWinCmd : public COutputBase
    {
    public:
        virtual ~COutputWinCmd() override;
        void log(const std::string& message, ELogLevel eLevel) override;

    protected:
        void write(const std::string& write) override;
    };
}