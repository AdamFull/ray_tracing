#include "OutputFile.h"

using namespace utl;

COutputFile::COutputFile(const std::string& filepath)
{
    file.open(filepath, std::ios_base::out | std::ios_base::binary);
}

COutputFile::~COutputFile()
{
    if(file.is_open())
        file.close();
}

void COutputFile::log(const std::string& message, ELogLevel eLevel)
{
    write(message);
}

void COutputFile::write(const std::string& write)
{
    file << write << std::endl;
    file.flush();
}