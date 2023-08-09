#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>
#include <cmath>
#include <cassert>

namespace math
{
	template<class _Ty>
	constexpr inline _Ty abs(_Ty _v) noexcept
	{
		return (_v < static_cast<_Ty>(0)) ? -_v : _v;
	}

	template<class _Ty>
	inline _Ty min(const _Ty& lhs, const _Ty& rhs) noexcept
	{
		return std::min(lhs, rhs);
	}

	template<class _Ty>
	inline _Ty max(const _Ty& lhs, const _Ty& rhs) noexcept
	{ 
		return std::max(lhs, rhs);
	}

	template<class _Ty>
	inline _Ty sqrt(_Ty& val)
	{
		return std::sqrt(val);
	}

	template<class _Ty>
	inline _Ty clamp(const _Ty& _x, const _Ty& _min, const _Ty& _max)
	{
		if (_x < _min) return _min;
		if (_x > _max) return _max;
		return _x;
	}
}