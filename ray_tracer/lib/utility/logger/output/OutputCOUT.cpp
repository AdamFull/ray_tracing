#include "OutputCOUT.h"

#include <iostream>
#include "formatter.h"

using namespace utl;

COutputCOUT::~COutputCOUT()
{

}

void COutputCOUT::log(const std::string& message, ELogLevel eLevel)
{
    write(formatter::colorize(eLevel, message));
}

void COutputCOUT::write(const std::string& write)
{
    std::cout << write << std::endl;
}