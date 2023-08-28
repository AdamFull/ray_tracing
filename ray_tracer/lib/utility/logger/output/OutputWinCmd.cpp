#include "OutputWinCmd.h"

#include "formatter.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using namespace utl;

COutputWinCmd::~COutputWinCmd()
{

}

void COutputWinCmd::log(const std::string& message, ELogLevel eLevel)
{
    write(message);
}

void COutputWinCmd::write(const std::string& write)
{
#ifdef _WIN32
    OutputDebugString((write + "\n").c_str());
#endif
}