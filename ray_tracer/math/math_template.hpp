#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>
#include <cmath>
#include <limits>
#include <cassert>

namespace math
{
	template<class _Ty>
	constexpr inline _Ty mabs(_Ty _v) noexcept
	{
		return (_v < static_cast<_Ty>(0)) ? -_v : _v;
	}

	template<class _Ty>
	constexpr inline _Ty min(const _Ty& lhs, const _Ty& rhs) noexcept
	{
		return lhs < rhs ? lhs : rhs;
	}

	template<class _Ty>
	constexpr inline _Ty max(const _Ty& lhs, const _Ty& rhs) noexcept
	{ 
		return lhs > rhs ? lhs : rhs;
	}

	template<class _Ty>
	inline _Ty sqrt(_Ty& val)
	{
		return std::sqrt(val);
	}

	template<class _Ty>
	constexpr inline _Ty clamp(const _Ty& _x, const _Ty& _min, const _Ty& _max) noexcept
	{
		if (_x < _min) return _min;
		if (_x > _max) return _max;
		return _x;
	}

	template<class _Ty>
	constexpr inline bool equal(const _Ty& lhs, const _Ty& rhs) noexcept
	{
		return lhs == rhs;
	}

	template<>
	constexpr inline bool equal<float>(const float& lhs, const float& rhs) noexcept
	{
		return mabs<float>(lhs - rhs) <= std::numeric_limits<float>::epsilon();
	}

	template<>
	constexpr inline bool equal<double>(const double& lhs, const double& rhs) noexcept
	{
		return mabs<double>(lhs - rhs) <= std::numeric_limits<double>::epsilon();
	}


	template<class _Ty>
	constexpr inline bool greather_equal(const _Ty& lhs, const _Ty& rhs) noexcept
	{
		return lhs >= rhs;
	}

	template<>
	constexpr inline bool greather_equal(const float& lhs, const float& rhs) noexcept
	{
		return (lhs > rhs) || equal(lhs, rhs);
	}

	template<>
	constexpr inline bool greather_equal(const double& lhs, const double& rhs) noexcept
	{
		return (lhs > rhs) || equal(lhs, rhs);
	}


	template<class _Ty>
	constexpr inline bool less_equal(const _Ty& lhs, const _Ty& rhs) noexcept
	{
		return lhs <= rhs;
	}

	template<>
	constexpr inline bool less_equal(const float& lhs, const float& rhs) noexcept
	{
		return (lhs < rhs) || equal(lhs, rhs);
	}

	template<>
	constexpr inline bool less_equal(const double& lhs, const double& rhs) noexcept
	{
		return (lhs < rhs) || equal(lhs, rhs);
	}
}