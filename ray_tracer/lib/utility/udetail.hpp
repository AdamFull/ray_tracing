#pragma once

#include <string_view>

namespace utl
{
    template <typename T> constexpr std::string_view type_name();

    template <>
    constexpr std::string_view type_name<void>() { return "void"; }

    namespace detail 
    {
        using type_name_prober = void;

        template <typename T>
        constexpr inline std::string_view wrapped_type_name() noexcept
        {
        #ifdef __clang__
            return __PRETTY_FUNCTION__;
        #elif defined(__GNUC__)
            return __PRETTY_FUNCTION__;
        #elif defined(_MSC_VER)
            return __FUNCSIG__;
        #else
        #error "Unsupported compiler"
        #endif
        }

        constexpr inline std::size_t wrapped_type_name_prefix_length() noexcept {
            return wrapped_type_name<type_name_prober>().find(type_name<type_name_prober>()); 
        }

        constexpr inline std::size_t wrapped_type_name_suffix_length() noexcept {
            return wrapped_type_name<type_name_prober>().length() 
                - wrapped_type_name_prefix_length() 
                - type_name<type_name_prober>().length();
        }

    } // namespace detail

    template <class _Ty>
    constexpr inline std::string_view type_name()
    {
        constexpr auto wrapped_name = detail::wrapped_type_name<_Ty>();
        constexpr auto prefix_length = detail::wrapped_type_name_prefix_length();
        constexpr auto suffix_length = detail::wrapped_type_name_suffix_length();
        constexpr auto type_name_length = wrapped_name.length() - prefix_length - suffix_length;
        return wrapped_name.substr(prefix_length, type_name_length);
    }

    constexpr inline uint32_t const_hash(char const* input) noexcept
    {
        return *input ? static_cast<uint32_t>(*input) + 33 * const_hash(input + 1) : 5381u;
    }

    constexpr inline uint32_t const_hash(char const* s, size_t count) noexcept
    {
        return ((count ? const_hash(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
    }

    constexpr inline uint32_t operator "" _utl_hash(char const* s, std::size_t count) noexcept
    {
        return const_hash(s, count);
    }

    template<class _Ty>
    constexpr inline uint32_t type_hash() noexcept
    {
        constexpr auto typestr = type_name<_Ty>();
        return const_hash(typestr.data());
    }
}