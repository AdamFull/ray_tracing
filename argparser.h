#pragma once

#include <optional>
#include <sstream>

class CArgumentParser
{
public:
    CArgumentParser(int& argc, char** argv);

    template<class _Ty>
    _Ty try_get(const std::string& option, _Ty default_value)
    {
        if (!exists(option))
            return default_value;

        auto svalue = get(option).value();

        if constexpr (std::is_same_v<_Ty, std::string>)
            return svalue;

        _Ty value;
        std::stringstream ss;
        ss << svalue;
        ss >> value;

        return value;
    }
    
    std::optional<std::string> get(const std::string& option) const;
    bool exists(const std::string& option) const;
private:
    std::vector<std::string> tokens;
};