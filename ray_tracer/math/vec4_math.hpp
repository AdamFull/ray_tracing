#pragma once

#include "vec4.hpp"
#include "share.hpp"

namespace math
{
	template<class _Ty>
	inline vec4<_Ty> min(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		res.x = min(lhs.x, rhs.x);
		res.y = min(lhs.y, rhs.y);
		res.z = min(lhs.z, rhs.z);
		res.w = min(lhs.w, rhs.w);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> max(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		res.x = max(lhs.x, rhs.x);
		res.y = max(lhs.y, rhs.y);
		res.z = max(lhs.z, rhs.z);
		res.w = max(lhs.w, rhs.w);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> sqrt(vec4<_Ty>& vec)
	{
		vec4<_Ty> res{};
		res.x = sqrt(vec.x);
		res.y = sqrt(vec.y);
		res.z = sqrt(vec.z);
		res.w = sqrt(vec.w);
		return res;
	}


	template<class _Ty>
	constexpr inline _Ty dot(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		_Ty res{ static_cast<_Ty>(0) };
		_vector_dot_product<_Ty, 3ull>(lhs.data, rhs.data, res);
		return res;
	}

	template<class _Ty>
	constexpr inline _Ty length2(const vec4<_Ty>& vec) noexcept
	{
		return dot(vec, vec);
	}

	template<class _Ty>
	inline _Ty length(const vec4<_Ty>& vec) noexcept
	{
		return sqrt(length2(vec));
	}

	template<class _Ty>
	inline vec4<_Ty> normalize(const vec4<_Ty>& vec)
	{
		vec4<_Ty> res{};
		res = vec / length(vec);
		return res;
	}
}