#pragma once

#include "vec4.hpp"
#include "share.hpp"

namespace math
{
	template<>
	inline vec4 min(const vec4& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		res.vec128 = _mm_min_ps(lhs.vec128, rhs.vec128);
#else
		res.x = std::min(lhs.x, rhs.x);
		res.y = std::min(lhs.y, rhs.y);
		res.z = std::min(lhs.z, rhs.z);
		res.w = std::min(lhs.w, rhs.w);
#endif
		return res;
	}

	template<>
	inline vec4 max(const vec4& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		res.vec128 = _mm_max_ps(lhs.vec128, rhs.vec128);
#else
		res.x = std::max(lhs.x, rhs.x);
		res.y = std::max(lhs.y, rhs.y);
		res.z = std::max(lhs.z, rhs.z);
		res.w = std::max(lhs.w, rhs.w);
#endif
		return res;
	}

	template<>
	inline vec4 sqrt(vec4& vec)
	{
		vec4 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		res.vec128 = _mm_sqrt_ps(vec.vec128);
#else
		res.x = std::sqrt(vec.x);
		res.y = std::sqrt(vec.y);
		res.z = std::sqrt(vec.z);
		res.w = std::sqrt(vec.w);
#endif
		return res;
	}


	inline float dot(const vec4& lhs, const vec4& rhs)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		return _mm_cvtss_f32(_vec128_dot_product(lhs.vec128, rhs.vec128));
#else
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w;
#endif
	}

	inline float length2(const vec4& vec) noexcept
	{
		return dot(vec, vec);
	}

	inline float length(const vec4& vec) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		return _mm_cvtss_f32(_vec128_length(vec.vec128));
#else
		return std::sqrt(length2(vec));
#endif
	}

	inline vec4 normalize(const vec4& vec)
	{
		vec4 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		res.vec128 = _vec128_normalize(vec.vec128);
#else
		auto len = length(vec);
		res = vec / len;
#endif
		return res;
	}
}