#pragma once

#include <fstream>
#include "OutputBase.h"

namespace utl
{
    class COutputFile : public COutputBase
    {
    public:
        COutputFile(const std::string& filepath);
        virtual ~COutputFile() override;
        void log(const std::string& message, ELogLevel eLevel) override;

    protected:
        void write(const std::string& write) override;
    private:
        std::ofstream file;
    };
}