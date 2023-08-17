#pragma once

#include "vec2.hpp"
#include "share.hpp"

namespace math
{
	template<class _Ty>
	constexpr inline vec2<_Ty> min(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		res.x = min(lhs.x, rhs.x);
		res.y = min(lhs.y, rhs.y);
		return res;
	}

	template<class _Ty>
	constexpr inline vec2<_Ty> max(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		res.x = max(lhs.x, rhs.x);
		res.y = max(lhs.y, rhs.y);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> sqrt(const vec2<_Ty>& vec)
	{
		vec2<_Ty> res{};
		res.x = sqrt(vec.x);
		res.y = sqrt(vec.y);
		return res;
	}

	template<class _Ty>
	constexpr inline _Ty dot(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		_Ty res{ static_cast<_Ty>(0) };
		_vector_dot_product<_Ty, 2ull>(lhs.data, rhs.data, res);
		return res;
	}

	template<class _Ty>
	constexpr inline _Ty length2(const vec2<_Ty>& vec) noexcept
	{
		return dot(vec, vec);
	}

	template<class _Ty>
	inline _Ty length(const vec2<_Ty>& vec) noexcept
	{
		return sqrt(length2(vec));
	}

	template<class _Ty>
	inline vec2<_Ty> normalize(const vec2<_Ty>& vec)
	{
		vec2<_Ty> res{};
		res = vec / length(vec);
		return res;
	}
}