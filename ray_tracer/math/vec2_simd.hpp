#pragma once

#include "vec2.hpp"

#include <xmmintrin.h>
#include <emmintrin.h>
#include <array>

namespace math
{
	namespace _impl
	{
		template<class _Ty>
		inline _Ty _parallel_dot_product_impl(const _Ty& v02, const _Ty& v13) noexcept {}

		template<>
		inline __m128 _parallel_dot_product_impl(const __m128& v02, const __m128& v13) noexcept
		{
			__m128 product = _mm_mul_ps(v02, v13);
			return _mm_add_ps(product, _mm_shuffle_ps(product, product, _MM_SHUFFLE(1, 0, 3, 2)));
		}

		template<>
		inline __m256 _parallel_dot_product_impl(const __m256& v0246, const __m256& v1357) noexcept
		{
			__m256 product = _mm256_mul_ps(v0246, v1357);
			return _mm256_add_ps(product, _mm256_shuffle_ps(product, product, _MM_SHUFFLE(1, 0, 3, 2)));
		}
	}

	template<class _Ty>
	struct parallel_batch_v2
	{
		parallel_batch_v2(const vec2<_Ty>& _v0, const vec2<_Ty>& _v1, const vec2<_Ty>& _v2, const vec2<_Ty>& _v3, const vec2<_Ty>& _v4, const vec2<_Ty>& _v5, const vec2<_Ty>& _v6, const vec2<_Ty>& _v7) 
			: v0(_v0), v1(_v1), v2(_v2), v3(_v3), v4(_v4), v5(_v5), v6(_v6), v7(_v7) {}

		const vec2<_Ty>& v0;
		const vec2<_Ty>& v1;
		const vec2<_Ty>& v2;
		const vec2<_Ty>& v3;
		const vec2<_Ty>& v4;
		const vec2<_Ty>& v5;
		const vec2<_Ty>& v6;
		const vec2<_Ty>& v7;
	};

	template<class _Ty>
	struct parallel_result_v2
	{
		std::array<_Ty, 4ull> data;

		const _Ty& operator[](size_t index) const { return data[index]; }
		_Ty& operator[](size_t index) { return data[index]; }
	};

	template<class _Ty>
	inline parallel_batch_v2<_Ty> make_parallel_batch(const vec2<_Ty>* data, size_t begin)
	{
		return parallel_batch_v2<_Ty>{data[begin], data[begin + 1ull], data[begin + 2ull], data[begin + 3ull], data[begin + 4ull], data[begin + 5ull], data[begin + 6ull], data[begin + 7ull]};
	}

	template<class _Ty>
	inline parallel_batch_v2<_Ty> make_parallel_batch(const vec2<_Ty>& v0, const vec2<_Ty>& v1, const vec2<_Ty>& v2, const vec2<_Ty>& v3, const vec2<_Ty>& v4, const vec2<_Ty>& v5, const vec2<_Ty>& v6, const vec2<_Ty>& v7)
	{
		return parallel_batch_v2<_Ty>{v0, v1, v2, v3, v4, v5, v6, v7};
	}

	template<class _Ty>
	inline parallel_batch_v2<_Ty> make_parallel_batch(const vec2<_Ty>& v0, const vec2<_Ty>& v1, const vec2<_Ty>& v2, const vec2<_Ty>& v3)
	{
		return parallel_batch_v2<_Ty>{v0, v0, v1, v1, v2, v2, v3, v3};
	}

	template<class _Ty>
	inline parallel_result_v2<vec2<_Ty>> parallel_add(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<vec2<float>> parallel_add(const parallel_batch_v2<float>& batch) noexcept
	{
		parallel_result_v2<vec2<float>> result;

		__m256 v0246 = _mm256_setr_ps(batch.v0.x, batch.v0.y, batch.v2.x, batch.v2.y, batch.v4.x, batch.v4.y, batch.v6.x, batch.v6.y);
		__m256 v1357 = _mm256_setr_ps(batch.v1.x, batch.v1.y, batch.v3.x, batch.v3.y, batch.v5.x, batch.v5.y, batch.v7.x, batch.v7.y);

		_mm256_store_ps((float*)(&result), _mm256_add_ps(v0246, v1357));

		return result;
	}

	template<class _Ty>
	inline parallel_result_v2<vec2<_Ty>> parallel_sub(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<vec2<float>> parallel_sub(const parallel_batch_v2<float>& batch) noexcept
	{
		parallel_result_v2<vec2<float>> result;

		__m256 v0246 = _mm256_setr_ps(batch.v0.x, batch.v0.y, batch.v2.x, batch.v2.y, batch.v4.x, batch.v4.y, batch.v6.x, batch.v6.y);
		__m256 v1357 = _mm256_setr_ps(batch.v1.x, batch.v1.y, batch.v3.x, batch.v3.y, batch.v5.x, batch.v5.y, batch.v7.x, batch.v7.y);

		_mm256_store_ps((float*)(&result), _mm256_sub_ps(v0246, v1357));

		return result;
	}

	template<class _Ty>
	inline parallel_result_v2<vec2<_Ty>>  parallel_mul(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<vec2<float>> parallel_mul(const parallel_batch_v2<float>& batch) noexcept
	{
		parallel_result_v2<vec2<float>> result;

		__m256 v0246 = _mm256_setr_ps(batch.v0.x, batch.v0.y, batch.v2.x, batch.v2.y, batch.v4.x, batch.v4.y, batch.v6.x, batch.v6.y);
		__m256 v1357 = _mm256_setr_ps(batch.v1.x, batch.v1.y, batch.v3.x, batch.v3.y, batch.v5.x, batch.v5.y, batch.v7.x, batch.v7.y);

		_mm256_store_ps((float*)(&result), _mm256_mul_ps(v0246, v1357));

		return result;
	}

	template<class _Ty>
	inline parallel_result_v2<vec2<_Ty>>  parallel_div(const parallel_batch_v2<_Ty>& batch) {}

	template<>
	inline parallel_result_v2<vec2<float>> parallel_div(const parallel_batch_v2<float>& batch)
	{
		parallel_result_v2<vec2<float>> result;

		__m256 v0246 = _mm256_setr_ps(batch.v0.x, batch.v0.y, batch.v2.x, batch.v2.y, batch.v4.x, batch.v4.y, batch.v6.x, batch.v6.y);
		__m256 v1357 = _mm256_setr_ps(batch.v1.x, batch.v1.y, batch.v3.x, batch.v3.y, batch.v5.x, batch.v5.y, batch.v7.x, batch.v7.y);

		_mm256_store_ps((float*)(&result), _mm256_div_ps(v0246, v1357));

		return result;
	}

	template<class _Ty>
	inline parallel_result_v2<_Ty>  parallel_dot_product(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<float> parallel_dot_product(const parallel_batch_v2<float>& batch) noexcept
	{
		__m256 v0246 = _mm256_setr_ps(batch.v0.x, batch.v2.x, batch.v4.x, batch.v6.x, batch.v0.y, batch.v2.y, batch.v4.y, batch.v6.y);
		__m256 v1357 = _mm256_setr_ps(batch.v1.x, batch.v3.x, batch.v5.x, batch.v7.x, batch.v1.y, batch.v3.y, batch.v5.y, batch.v7.y);

		__m256 product = _mm256_mul_ps(v0246, v1357);

		__m128 vx = _mm256_extractf128_ps(product, 0);
		__m128 vy = _mm256_extractf128_ps(product, 1);

		parallel_result_v2<float> result;
		_mm_storeu_ps((float*)(&result), _mm_add_ps(vx, vy));

		return result;
	}

	template<class _Ty>
	inline parallel_result_v2<_Ty> parallel_length2(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<float> parallel_length2(const parallel_batch_v2<float>& batch) noexcept
	{
		return parallel_dot_product(batch);
	}

	template<class _Ty>
	inline parallel_result_v2<_Ty> parallel_length(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<float> parallel_length(const parallel_batch_v2<float>& batch) noexcept
	{
		__m128 v02 = _mm_setr_ps(batch.v0.x, batch.v2.x, batch.v0.y, batch.v2.y);
		__m128 v13 = _mm_setr_ps(batch.v1.x, batch.v3.x, batch.v1.y, batch.v3.y);

		float raw[4ull];
		_mm_store_ps(raw, _mm_sqrt_ps(_impl::_parallel_dot_product_impl(v02, v13)));

		parallel_result_v2<float> result;
		result[0ull] = raw[0ull];
		result[1ull] = raw[1ull];

		return result;
	}

	template<class _Ty>
	inline parallel_result_v2<_Ty> parallel_normalize(const parallel_batch_v2<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v2<float> parallel_normalize(const parallel_batch_v2<float>& batch) noexcept
	{
		__m128 v02 = _mm_setr_ps(batch.v0.x, batch.v2.x, batch.v0.y, batch.v2.y);
		__m128 v13 = _mm_setr_ps(batch.v1.x, batch.v3.x, batch.v1.y, batch.v3.y);
	
		float raw[4ull];
		_mm_store_ps(raw, _mm_div_ps(v02, _mm_sqrt_ps(_impl::_parallel_dot_product_impl(v02, v13))));

		parallel_result_v2<float> result;
		result[0ull] = raw[0ull];
		result[1ull] = raw[3ull];

		return result;
	}
}