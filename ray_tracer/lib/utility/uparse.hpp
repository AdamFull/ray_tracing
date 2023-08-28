#pragma once
#include "json.hpp"
#include <optional>
#include <set>

namespace utl
{
    template<class _Ty>
    inline void parse_from(const std::string& name, const nlohmann::json &json, _Ty& type, bool required = false)
    {
        if (auto obj = json.find(name); obj != json.end())
            type = obj->get<_Ty>();
        else
            if (required)
                throw std::runtime_error("Required argument was not found");
    }

    template<class _Ty>
    inline void serialize_to(const std::string& name, nlohmann::json& json, const _Ty& type, bool condition)
    {
        if (condition)
            json[name] = type;
    }
}

namespace nlohmann
{
    template <typename _Ty>
    struct adl_serializer<std::optional<_Ty>> 
    {
        static void to_json(json& j, const std::optional<_Ty>& opt) 
        {
            if (!opt.has_value())
                j = nullptr;
            else
                j = opt.value();
        }

        static void from_json(const json& j, std::optional<_Ty>& opt) 
        {
            if (j.is_null())
                opt = std::nullopt;
            else
                opt = j.get<_Ty>();
        }
    };
}