#pragma once

#include "vec3.hpp"

#include <xmmintrin.h>
#include <emmintrin.h>
#include <array>

namespace math
{
	template<class _Ty>
	struct parallel_batch_v3
	{
		parallel_batch_v3(const vec3<_Ty>& _v0, const vec3<_Ty>& _v1, const vec3<_Ty>& _v2, const vec3<_Ty>& _v3, const vec3<_Ty>& _v4, const vec3<_Ty>& _v5, const vec3<_Ty>& _v6, const vec3<_Ty>& _v7) :
			v0(_v0), v1(_v1), v2(_v2), v3(_v3), v4(_v4), v5(_v5), v6(_v6), v7(_v7) {}

		const vec3<_Ty>& v0;
		const vec3<_Ty>& v1;
		const vec3<_Ty>& v2;
		const vec3<_Ty>& v3;
		const vec3<_Ty>& v4;
		const vec3<_Ty>& v5;
		const vec3<_Ty>& v6;
		const vec3<_Ty>& v7;
	};

	template<class _Ty>
	struct parallel_result_v3
	{
		std::array<vec3<_Ty>, 4ull> data;

		const vec3<_Ty>& operator[](size_t index) const { return data[index]; }
		vec3<_Ty>& operator[](size_t index) { return data[index]; }
	};

	template<class _Ty>
	parallel_batch_v3<_Ty> make_parallel_batch(const vec3<_Ty>* data, size_t begin)
	{
		return parallel_batch_v3<_Ty>{data[begin], data[begin + 1ull], data[begin + 2ull], data[begin + 3ull], data[begin + 4ull], data[begin + 5ull], data[begin + 6ull], data[begin + 7ull]};
	}

	template<class _Ty>
	parallel_batch_v3<_Ty> make_parallel_batch(const vec3<_Ty>& v0, const vec3<_Ty>& v1, const vec3<_Ty>& v2, const vec3<_Ty>& v3, const vec3<_Ty>& v4, const vec3<_Ty>& v5, const vec3<_Ty>& v6, const vec3<_Ty>& v7)
	{
		return parallel_batch_v3<_Ty>{v0, v1, v2, v3, v4, v5, v6, v7};
	}

	template<class _Ty>
	parallel_batch_v3<_Ty> make_parallel_batch(const vec3<_Ty>& v0, const vec3<_Ty>& v1, const vec3<_Ty>& v2, const vec3<_Ty>& v3)
	{
		return parallel_batch_v3<_Ty>{v0, v0, v1, v1, v2, v2, v3, v3};
	}

	template<class _Ty>
	inline parallel_result_v3<_Ty> parallel_add(const parallel_batch_v3<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v3<float> parallel_add(const parallel_batch_v3<float>& batch) noexcept
	{
		parallel_result_v3<float> result;

		__m256 v0246xy = _mm256_setr_ps(batch.v0.x, batch.v2.x, batch.v0.y, batch.v2.y, batch.v4.x, batch.v6.x, batch.v4.y, batch.v6.y);
		__m256 v1357xy = _mm256_setr_ps(batch.v1.x, batch.v3.x,batch.v1.y, batch.v3.y, batch.v5.x, batch.v7.x, batch.v5.y, batch.v7.y);
		__m128 v0246z = _mm_setr_ps(batch.v0.z, batch.v2.z, batch.v4.z, batch.v6.z);
		__m128 v1357z = _mm_setr_ps(batch.v1.z, batch.v3.z, batch.v5.z, batch.v6.z);

		__m256 v0246r1357xy = _mm256_add_ps(v0246xy, v1357xy);
		__m128 v0246r1357z = _mm_add_ps(v0246z, v1357z);

		float r0[8ull], r1[4ull];
		_mm256_store_ps(r0, v0246r1357xy);
		_mm_store_ps(r1, v0246r1357z);

		result[0ull] = vec3<float>(r0[0ull], r0[2ull], r1[0ull]);
		result[1ull] = vec3<float>(r0[1ull], r0[3ull], r1[1ull]);
		result[2ull] = vec3<float>(r0[4ull], r0[6ull], r1[2ull]);
		result[3ull] = vec3<float>(r0[5ull], r0[7ull], r1[3ull]);

		return result;
	}

	template<class _Ty>
	inline parallel_result_v3<_Ty> parallel_sub(const parallel_batch_v3<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v3<float> parallel_sub(const parallel_batch_v3<float>& batch) noexcept
	{
		parallel_result_v3<float> result;

		__m256 v0246xy = _mm256_setr_ps(batch.v0.x, batch.v2.x, batch.v0.y, batch.v2.y, batch.v4.x, batch.v6.x, batch.v4.y, batch.v6.y);
		__m256 v1357xy = _mm256_setr_ps(batch.v1.x, batch.v3.x,batch.v1.y, batch.v3.y, batch.v5.x, batch.v7.x, batch.v5.y, batch.v7.y);
		__m128 v0246z = _mm_setr_ps(batch.v0.z, batch.v2.z, batch.v4.z, batch.v6.z);
		__m128 v1357z = _mm_setr_ps(batch.v1.z, batch.v3.z, batch.v5.z, batch.v6.z);

		__m256 v0246r1357xy = _mm256_sub_ps(v0246xy, v1357xy);
		__m128 v0246r1357z = _mm_sub_ps(v0246z, v1357z);

		float r0[8ull], r1[4ull];
		_mm256_store_ps(r0, v0246r1357xy);
		_mm_store_ps(r1, v0246r1357z);

		result[0ull] = vec3<float>(r0[0ull], r0[2ull], r1[0ull]);
		result[1ull] = vec3<float>(r0[1ull], r0[3ull], r1[1ull]);
		result[2ull] = vec3<float>(r0[4ull], r0[6ull], r1[2ull]);
		result[3ull] = vec3<float>(r0[5ull], r0[7ull], r1[3ull]);

		return result;
	}

	template<class _Ty>
	inline parallel_result_v3<_Ty> parallel_mul(const parallel_batch_v3<_Ty>& batch) noexcept {}

	template<>
	inline parallel_result_v3<float> parallel_mul(const parallel_batch_v3<float>& batch) noexcept
	{
		parallel_result_v3<float> result;

		__m256 v0246xy = _mm256_setr_ps(batch.v0.x, batch.v2.x, batch.v0.y, batch.v2.y, batch.v4.x, batch.v6.x, batch.v4.y, batch.v6.y);
		__m256 v1357xy = _mm256_setr_ps(batch.v1.x, batch.v3.x,batch.v1.y, batch.v3.y, batch.v5.x, batch.v7.x, batch.v5.y, batch.v7.y);
		__m128 v0246z = _mm_setr_ps(batch.v0.z, batch.v2.z, batch.v4.z, batch.v6.z);
		__m128 v1357z = _mm_setr_ps(batch.v1.z, batch.v3.z, batch.v5.z, batch.v6.z);

		__m256 v0246r1357xy = _mm256_mul_ps(v0246xy, v1357xy);
		__m128 v0246r1357z = _mm_mul_ps(v0246z, v1357z);

		float r0[8ull], r1[4ull];
		_mm256_store_ps(r0, v0246r1357xy);
		_mm_store_ps(r1, v0246r1357z);

		result[0ull] = vec3<float>(r0[0ull], r0[2ull], r1[0ull]);
		result[1ull] = vec3<float>(r0[1ull], r0[3ull], r1[1ull]);
		result[2ull] = vec3<float>(r0[4ull], r0[6ull], r1[2ull]);
		result[3ull] = vec3<float>(r0[5ull], r0[7ull], r1[3ull]);

		return result;
	}

	template<class _Ty>
	inline parallel_result_v3<_Ty> parallel_div(const parallel_batch_v3<_Ty>& batch) {}

	template<>
	inline parallel_result_v3<float> parallel_div(const parallel_batch_v3<float>& batch)
	{
		parallel_result_v3<float> result;

		__m256 v0246xy = _mm256_setr_ps(batch.v0.x, batch.v2.x, batch.v0.y, batch.v2.y, batch.v4.x, batch.v6.x, batch.v4.y, batch.v6.y);
		__m256 v1357xy = _mm256_setr_ps(batch.v1.x, batch.v3.x,batch.v1.y, batch.v3.y, batch.v5.x, batch.v7.x, batch.v5.y, batch.v7.y);
		__m128 v0246z = _mm_setr_ps(batch.v0.z, batch.v2.z, batch.v4.z, batch.v6.z);
		__m128 v1357z = _mm_setr_ps(batch.v1.z, batch.v3.z, batch.v5.z, batch.v6.z);

		__m256 v0246r1357xy = _mm256_div_ps(v0246xy, v1357xy);
		__m128 v0246r1357z = _mm_div_ps(v0246z, v1357z);

		float r0[8ull], r1[4ull];
		_mm256_store_ps(r0, v0246r1357xy);
		_mm_store_ps(r1, v0246r1357z);

		result[0ull] = vec3<float>(r0[0ull], r0[2ull], r1[0ull]);
		result[1ull] = vec3<float>(r0[1ull], r0[3ull], r1[1ull]);
		result[2ull] = vec3<float>(r0[4ull], r0[6ull], r1[2ull]);
		result[3ull] = vec3<float>(r0[5ull], r0[7ull], r1[3ull]);

		return result;
	}

}