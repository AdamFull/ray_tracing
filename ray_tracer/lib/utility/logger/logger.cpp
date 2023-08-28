#include "logger.h"

using namespace utl;

void CLogger::init(const std::string& app_name, const std::string& app_version)
{
    for(auto& output : vOutputs)
        output->init(app_name, app_version);
}

void CLogger::addOutput(std::shared_ptr<COutputBase>&& output)
{
    vOutputs.emplace_back(std::move(output));
}