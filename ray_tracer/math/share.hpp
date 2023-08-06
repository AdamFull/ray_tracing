#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>

namespace math
{
	inline __m128 _vec128_dot_product(const __m128& lhs, const __m128& rhs)
	{
		__m128 mul, shuffle, sum;
		mul = _mm_mul_ps(lhs, rhs);

		shuffle = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(2, 3, 0, 1));
		sum = _mm_add_ps(mul, shuffle);
		shuffle = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 1, 2, 3));

		return _mm_add_ss(sum, shuffle);
	}

	inline __m128 _vec128_length_sq(const __m128& vec)
	{
		return _vec128_dot_product(vec, vec);
	}

	inline __m128 _vec128_length(const __m128& vec)
	{
		__m128 len_sq = _vec128_length_sq(vec);
		return _mm_div_ss(_mm_set_ps1(1.f), _mm_rsqrt_ss(len_sq));
	}

	inline __m128 _vec128_normalize(const __m128& vec)
	{
		__m128 len_sq = _vec128_length_sq(vec);
		__m128 rlen = _mm_rsqrt_ss(len_sq);
		rlen = _mm_shuffle_ps(rlen, rlen, _MM_SHUFFLE(0, 0, 0, 0));
		return _mm_mul_ps(vec, rlen);
	}

	inline __m128 _vec128_cross(const __m128& lhs, const __m128& rhs)
	{
		//__m128 tmp0 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 0, 2, 1));
		//__m128 tmp1 = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
		//tmp0 = _mm_mul_ps(tmp0, lhs);
		//tmp1 = _mm_mul_ps(tmp1, rhs);
		//__m128 tmp2 = _mm_sub_ps(tmp0, tmp1);
		//
		//return _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));

		__m128 tmp0 = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 tmp1 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 tmp2 = _mm_mul_ps(tmp0, rhs);
		__m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
		__m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
		return _mm_sub_ps(tmp3, tmp4);
	}
}