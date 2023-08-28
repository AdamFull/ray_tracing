#include "OutputBase.h"

#include "formatter.h"

using namespace utl;

void COutputBase::init(const std::string& app_name, const std::string& app_version)
{
    write("#Software: " + app_name);
    write("#Version: " + app_version);
    write("#Date: " + formatter::get_formatted_datetime());
    write("#Fields: level time file function line message");
}