#pragma once
#include <vector>
#include <string>

namespace utl
{
    struct base64
    {
        static std::string encode(uint8_t const *buf, unsigned int bufLen);
        static std::vector<uint8_t> decode(std::string const &);

        static inline bool is_base64(uint8_t c) {
            return (isalnum(c) || (c == '+') || (c == '/'));
        }
    };
}
