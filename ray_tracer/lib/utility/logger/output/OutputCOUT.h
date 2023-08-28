#pragma once

#include "OutputBase.h"

namespace utl
{
    class COutputCOUT : public COutputBase
    {
    public:
        virtual ~COutputCOUT() override;
        void log(const std::string& message, ELogLevel eLevel) override;

    protected:
        void write(const std::string& write) override;
    };
}