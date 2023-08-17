#pragma once

#include "vec4.hpp"

#include <xmmintrin.h>
#include <emmintrin.h>
#include <array>

namespace math
{
	template<class _KTy, class _Ty>
	std::array<_KTy, 4ull> parallel_load(const vec4<_Ty>& v0, const vec4<_Ty>& v1, const vec4<_Ty>& v2, const vec4<_Ty>& v3)
	{

	}

	template<>
	std::array<__m128, 4ull> parallel_load(const vec4<float>& v0, const vec4<float>& v1, const vec4<float>& v2, const vec4<float>& v3)
	{
		std::array<__m128, 4ull> out{};
		out[0ull] = _mm_setr_ps(v0.x, v1.x, v2.x, v3.x);
		out[1ull] = _mm_setr_ps(v0.y, v1.y, v2.y, v3.y);
		out[2ull] = _mm_setr_ps(v0.z, v1.z, v2.z, v3.z);
		out[3ull] = _mm_setr_ps(v0.w, v1.w, v2.w, v3.w);
		return out;
	}
}