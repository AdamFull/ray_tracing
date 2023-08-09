#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>

#define MAKE_SSE_VECTOR3(name) struct { union  { struct { glm::vec3 m_##name; float m_align_##name; }; __m128 m_vec128_##name; }; };

namespace math
{
	template<class _Ty>
	constexpr inline _Ty abs(_Ty _v) noexcept
	{
		return (_v < static_cast<_Ty>(0)) ? -_v : _v;
	}

	constexpr inline bool compare_float(const float& lhs, const float& rhs) noexcept
	{
		return abs(lhs - rhs) <= std::numeric_limits<float>::epsilon();
	}

	constexpr inline bool greater_equal_float(const float& lhs, const float& rhs)
	{
		return (lhs > rhs) || compare_float(lhs, rhs);
	}

	constexpr inline bool less_equal_float(const float& lhs, const float& rhs)
	{
		return (lhs < rhs) || compare_float(lhs, rhs);
	}


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
		__m128 tmp0 = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 tmp1 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 tmp2 = _mm_mul_ps(tmp0, rhs);
		__m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
		__m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
		return _mm_sub_ps(tmp3, tmp4);
	}




	inline glm::vec3 apply_otb(const glm::vec3& t, const glm::vec3& b, const glm::vec3& n, const glm::vec3& v)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		__m128 x = _mm_mul_ps(_mm_shuffle_ps(v.vec128, v.vec128, _MM_SHUFFLE(0, 0, 0, 0)), t.vec128);
		__m128 y = _mm_mul_ps(_mm_shuffle_ps(v.vec128, v.vec128, _MM_SHUFFLE(1, 1, 1, 1)), b.vec128);
		__m128 z = _mm_mul_ps(_mm_shuffle_ps(v.vec128, v.vec128, _MM_SHUFFLE(2, 2, 2, 2)), n.vec128);
#endif

		glm::vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_OPERATIONS)
		res.vec128 = _mm_add_ps(x, _mm_add_ps(y, z));
#else
		res = v.x * t + v.y * b + v.z * n;
#endif
		return res;
	}

	inline glm::vec3 min(const glm::vec3& lhs, const glm::vec3& rhs)
	{
#if defined(USE_INTRINSICS) && 0
		auto _lhs = _mm_setr_ps(lhs.x, lhs.y, lhs.z, 0.f);
		auto _rhs = _mm_setr_ps(rhs.x, rhs.y, rhs.z, 0.f);

		auto _res = _mm_min_ps(_lhs, _rhs);
		return glm::vec3(_res.m128_f32[0], _res.m128_f32[1], _res.m128_f32[2]);
#else
		return glm::min(lhs, rhs);
#endif
	}

	inline glm::vec3 max(const glm::vec3& lhs, const glm::vec3& rhs)
	{
#if defined(USE_INTRINSICS) && 0
		auto _lhs = _mm_setr_ps(lhs.x, lhs.y, lhs.z, 0.f);
		auto _rhs = _mm_setr_ps(rhs.x, rhs.y, rhs.z, 0.f);

		auto _res = _mm_max_ps(_lhs, _rhs);
		return glm::vec3(_res.m128_f32[0], _res.m128_f32[1], _res.m128_f32[2]);
#else
		return glm::max(lhs, rhs);
#endif
	}

	inline float ray_aabb_intersect_impl_sse(const __m128& ray_origin, const __m128& inv_ray_dir, const __m128& bmin, const __m128& bmax, float distance) noexcept
	{
		static __m128 mask = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_set_ps(1.f, 0.f, 0.f, 0.f));

		__m128 t1 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(bmin, mask), ray_origin), inv_ray_dir);
		__m128 t2 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(bmax, mask), ray_origin), inv_ray_dir);

		__m128 vmax = _mm_max_ps(t1, t2);
		__m128 vmin = _mm_min_ps(t1, t2);

		float tmax = glm::min(vmax.m128_f32[0], glm::min(vmax.m128_f32[1], vmax.m128_f32[2]));
		float tmin = glm::max(vmin.m128_f32[0], glm::max(vmin.m128_f32[1], vmin.m128_f32[2]));

		if (tmax >= tmin && tmin < distance && tmax > 0.f)
			return tmin;

		return std::numeric_limits<float>::max();
	}

	inline float ray_aabb_intersect_impl_base(const glm::vec3& r0, const glm::vec3& ird, const glm::vec3& bmin, const glm::vec3& bmax, float distance) noexcept
	{
		glm::vec3 vt1 = (bmin - r0) * ird;
		glm::vec3 vt2 = (bmax - r0) * ird;

		float tmin = glm::max(glm::max(glm::min(vt1.x, vt2.x), glm::min(vt1.y, vt2.y)), glm::min(vt1.z, vt2.z));
		float tmax = glm::min(glm::min(glm::max(vt1.x, vt2.x), glm::max(vt1.y, vt2.y)), glm::max(vt1.z, vt2.z));

		if (tmax >= tmin && tmin < distance && tmax > 0.f)
			return tmin;

		return std::numeric_limits<float>::max();
	}

	inline float ray_aabb_intersect(const glm::vec3& r0, const glm::vec3& ird, const glm::vec3& bmin, const glm::vec3& bmax, float distance) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_INTERSECTION) && 0
		auto _r0 = _mm_setr_ps(r0.x, r0.y, r0.z, 0.f);
		auto _ird = _mm_setr_ps(ird.x, ird.y, ird.z, 0.f);
		auto _bmin = _mm_setr_ps(bmin.x, bmin.y, bmin.z, 0.f);
		auto _bmax = _mm_setr_ps(bmax.x, bmax.y, bmax.z, 0.f);
		return ray_aabb_intersect_impl_sse(_r0, _ird, _bmin, _bmax, distance);
#else
		return ray_aabb_intersect_impl_base(r0, ird, bmin, bmax, distance);
#endif

	}

	// Triangle intersect

	inline bool ray_triangle_intersect_impl_sse(const __m128& r0, const __m128& rd, const __m128& v0, const __m128& v1, const __m128& v2, float& distance, glm::vec3& barycentric)
	{
		__m128 e0 = _mm_sub_ps(v1, v0);
		__m128 e1 = _mm_sub_ps(v2, v0);

		__m128 pvec = _vec128_cross(rd, e1);
		__m128 det = _vec128_dot_product(e0, pvec);

		if (_mm_cvtss_f32(det) < std::numeric_limits<float>::epsilon())
			return false;

		__m128 inv_det = _mm_div_ss(_mm_set_ps1(1.f), det);

		__m128 tvec = _mm_sub_ps(r0, v0);
		barycentric.y = _mm_cvtss_f32(_mm_mul_ss(_vec128_dot_product(tvec, pvec), inv_det));

		if (barycentric.y < 0.f || barycentric.y > 1.f)
			return false;

		__m128 qvec = _vec128_cross(tvec, e0);
		barycentric.z = _mm_cvtss_f32(_mm_mul_ss(_vec128_dot_product(rd, qvec), inv_det));

		if (barycentric.z < 0.f || barycentric.y + barycentric.z > 1.f)
			return false;

		barycentric.x = 1.f - barycentric.y - barycentric.z;

		distance = _mm_cvtss_f32(_mm_mul_ss(_vec128_dot_product(e1, qvec), inv_det));

		return true;
	}

	inline bool ray_triangle_intersect_impl_base(const glm::vec3& r0, const glm::vec3& rd, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& distance, glm::vec3& barycentric)
	{
		auto e0 = v1 - v0;
		auto e1 = v2 - v0;

		auto pvec = glm::cross(rd, e1);
		auto det = glm::dot(e0, pvec);

		if (det < std::numeric_limits<float>::epsilon())
			return false;

		auto inv_det = 1.f / det;

		auto tvec = r0 - v0;
		barycentric.y = glm::dot(tvec, pvec) * inv_det;

		if (barycentric.y < 0.f || barycentric.y > 1.f)
			return false;

		auto qvec = glm::cross(tvec, e0);
		barycentric.z = glm::dot(rd, qvec) * inv_det;

		if (barycentric.z < 0.f || barycentric.y + barycentric.z > 1.f)
			return false;

		barycentric.x = 1.f - barycentric.y - barycentric.z;

		distance = glm::dot(e1, qvec) * inv_det;

		return true;
	}

	inline bool ray_triangle_intersect(const glm::vec3& r0, const glm::vec3& rd, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& distance, glm::vec3& barycentric)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_INTERSECTION)
		auto _r0 = _mm_setr_ps(r0.x, r0.y, r0.z, 0.f);
		auto _rd = _mm_setr_ps(rd.x, rd.y, rd.z, 0.f);
		auto _v0 = _mm_setr_ps(v0.x, v0.y, v0.z, 0.f);
		auto _v1 = _mm_setr_ps(v1.x, v1.y, v1.z, 0.f);
		auto _v2 = _mm_setr_ps(v2.x, v2.y, v2.z, 0.f);
		return ray_triangle_intersect_impl_sse(_r0, _rd, _v0, _v1, _v2, distance, barycentric);
#else
		return ray_triangle_intersect_impl_base(r0, rd, v0, v1, v2, distance, barycentric);
#endif
	}
}