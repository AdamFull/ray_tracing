#pragma once

#include "vec2.hpp"
#include "share.hpp"

namespace math
{
	template<>
	inline vec2 min(const vec2& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_min_ps(lhs.vec128, rhs.vec128);
#else
		res.x = std::min(lhs.x, rhs.x);
		res.y = std::min(lhs.y, rhs.y);
#endif
		return res;
	}

	template<>
	inline vec2 max(const vec2& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_max_ps(lhs.vec128, rhs.vec128);
#else
		res.x = std::max(lhs.x, rhs.x);
		res.y = std::max(lhs.y, rhs.y);
#endif
		return res;
	}

	template<>
	inline vec2 sqrt(vec2& vec)
	{
		vec2 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_sqrt_ps(vec.vec128);
#else
		res.x = std::sqrt(vec.x);
		res.y = std::sqrt(vec.y);
#endif
		return res;
	}


	inline float dot(const vec2& lhs, const vec2& rhs)
	{
#if defined(USE_INTRINSICS)
		return _mm_cvtss_f32(_vec128_dot_product(lhs.vec128, rhs.vec128));
#else
		return lhs.x * rhs.x + lhs.y * rhs.y;
#endif
	}

	inline float length2(const vec2& vec) noexcept
	{
		return dot(vec, vec);
	}

	inline float length(const vec2& vec) noexcept
	{
#if defined(USE_INTRINSICS)
		return _mm_cvtss_f32(_vec128_length(vec.vec128));
#else
		return std::sqrt(length2(vec));
#endif
	}

	inline vec2 normalize(const vec2& vec)
	{
		vec2 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _vec128_normalize(vec.vec128);
#else
		auto len = length(vec);
		res = vec / len;
#endif
		return res;
	}
}