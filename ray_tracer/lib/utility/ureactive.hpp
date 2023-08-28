#pragma once

#include <optional>
#include "ufunction.hpp"
#include <numeric>
#include <stdint.h>
#include <variant>
#include <type_traits>
#include <cmath>
#include <tuple>

namespace utl
{
    using avaliable_types_t = std::tuple<int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, bool, float, double, std::string>;

    template <class T>
    struct variable
    {
        static_assert(!std::is_same<T, avaliable_types_t>::value,
                      "Placed type is not avaliable. variable<T> support only types: int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, bool, float, double.");

    public:
        variable() = default;

        variable(const T &val) { value = val; }

        template <class... _Types>
        variable(_Types... args) { bind(std::forward<_Types>(args)...); }

        ~variable() { notify = nullptr; }

        inline const std::optional<T> &get() const { return value; }

        template <class... _Types>
        void bind(_Types... args)
        {
            notify = std::move(function<void(const T &, const T &)>(std::forward<_Types>(args)...));
        }

        inline T &operator=(const T &val) noexcept
        {
            if (notify && !ncompare(last_changed, val))
            {
                notify(last_changed, val);
                last_changed = value.value();
                value = val;
            }
            return last_changed;
        }

        inline T &operator+=(const T &val)
        {
            return operator=(val + value.value());
        }

    private:
        template <typename U = T>
        typename std::enable_if<std::is_floating_point<U>::value, bool>::type
        ncompare(const T &first, const T &second)
        {
            return std::fabs(first - second) <= std::numeric_limits<T>::epsilon();
        }

        template <typename U = T>
        typename std::enable_if<!std::is_floating_point<U>::value, bool>::type
        ncompare(const T &first, const T &second)
        {
            return first == second;
        }

        T last_changed{NULL};
        std::optional<T> value{T()};
        function<void(const T &, const T &)> notify{nullptr};
    };

    using rint8_t = variable<int8_t>;
    using ruint8_t = variable<uint8_t>;
    using rint16_t = variable<int16_t>;
    using ruint16_t = variable<uint16_t>;
    using rint32_t = variable<int32_t>;
    using ruint32_t = variable<uint32_t>;
    using rint64_t = variable<int64_t>;
    using ruint64_t = variable<uint64_t>;

    using rbool = variable<bool>;

    using rfloat = variable<float>;
    using rdouble = variable<double>;

    using rstring = variable<std::string>;

    using variants = std::variant<rint8_t, ruint8_t, rint16_t, ruint16_t, rint32_t, ruint32_t, rint64_t, ruint64_t, rbool, rfloat, rdouble, rstring>;

    template <class T, class _variant>
    struct is_variant_type;

    template <class T, class... _Types>
    struct is_variant_type<T, std::variant<_Types...>>
        : public std::disjunction<std::is_same<T, _Types>...>
    {
    };

    template <class T>
    using is_react_value = is_variant_type<T, variants>;
}