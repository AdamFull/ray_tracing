#pragma once

#include <xmmintrin.h>
#include <emmintrin.h>
#include <immintrin.h>

#define MAKE_SSE_VECTOR3(name) struct { union  { struct { glm::vec3 m_##name; float m_align_##name; }; __m128 m_vec128_##name; }; };

namespace math
{
	extern "C" double sqrt14(double n);

	inline float fsqrt(float n)
	{
		//unsigned int i = *(unsigned int*)&n;
		//i += 127 << 23;
		//i >>= 1;
		//return *(float*)&i;
		return std::sqrt(n);
		//return sqrt14(n);
	}

	template<class _Ty>
	constexpr inline _Ty abs(_Ty _v) noexcept
	{
		return (_v < static_cast<_Ty>(0)) ? -_v : _v;
	}

	constexpr inline bool compare_float(const float& lhs, const float& rhs) noexcept
	{
		//return abs(lhs - rhs) <= std::numeric_limits<float>::epsilon();
		return lhs == rhs;
	}

	constexpr inline bool greater_equal_float(const float& lhs, const float& rhs)
	{
		//return (lhs > rhs) || compare_float(lhs, rhs);
		return lhs >= rhs;
	}

	constexpr inline bool less_equal_float(const float& lhs, const float& rhs)
	{
		//return (lhs < rhs) || compare_float(lhs, rhs);
		return lhs <= rhs;
	}

	constexpr inline float max_component(const glm::vec3& v) noexcept
	{
		return glm::max(v.x, glm::max(v.y, v.z));
	}

	inline bool isnan(const glm::vec3& v) noexcept
	{
		return std::isnan(v.x) || std::isnan(v.y) || std::isnan(v.z);
	}

	inline bool isfinite(const glm::vec3& v) noexcept
	{
		return std::isfinite(v.x) || std::isfinite(v.y) || std::isfinite(v.z);
	}

	inline float sign(float a) noexcept
	{
		return std::copysign(1.f, a);
	}

	template <typename _Ty>
	inline constexpr const glm::vec<3, _Ty, glm::defaultp> reflect(const glm::vec<3, _Ty, glm::defaultp>& wi, const glm::vec<3, _Ty, glm::defaultp>& n) 
	{
		return static_cast<_Ty>(2) * glm::dot(wi, n) * n - wi;
	}

	template<class _Ty>
	constexpr inline _Ty remap(_Ty value, _Ty low1, _Ty high1, _Ty low2, _Ty high2) noexcept
	{
		return glm::clamp(low2 + (value - low1) * (high2 - low2) / (high1 - low1), low2, high2);
	}

	template<class _Ty>
	inline bool refract(const glm::vec<3, _Ty, glm::defaultp>& wi, const glm::vec<3, _Ty, glm::defaultp>& n, _Ty eta, glm::vec<3, _Ty, glm::defaultp>& wt)
	{
		_Ty cosThetaI = glm::dot(n, wi);
		_Ty sin2ThetaI = glm::max(static_cast<_Ty>(0), static_cast<_Ty>(1) - cosThetaI * cosThetaI);
		_Ty sin2ThetaT = eta * eta * sin2ThetaI;
	
		if (sin2ThetaT >= static_cast<_Ty>(1))
			return false;
	
		_Ty cosThetaT = glm::sqrt(static_cast<_Ty>(1) - sin2ThetaT);
		wt = eta * -wi + (eta * cosThetaI - cosThetaT) * n;
		return true;
	}

	template<class _Ty>
	constexpr glm::vec<3, _Ty, glm::defaultp> lerp(const glm::vec<3, _Ty, glm::defaultp>& a, const glm::vec<3, _Ty, glm::defaultp>& b, _Ty t)
	{
		return (static_cast<_Ty>(1) - t) * a + t * b;
	}

	template<class _Ty>
	constexpr bool is_normalized(const glm::vec<3, _Ty, glm::defaultp>& a)
	{
		return glm::abs(glm::length2(a) - static_cast<_Ty>(1)) < static_cast<_Ty>(1.0e-6);
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

	//inline __m128 _vec128_length_sq(const __m128& vec)
	//{
	//	return _vec128_dot_product(vec, vec);
	//}

	//inline __m128 _vec128_length(const __m128& vec)
	//{
	//	__m128 len_sq = _vec128_length_sq(vec);
	//	return _mm_div_ss(_mm_set_ps1(1.f), _mm_rsqrt_ss(len_sq));
	//}

	//inline __m128 _vec128_normalize(const __m128& vec)
	//{
	//	__m128 len_sq = _vec128_length_sq(vec);
	//	__m128 rlen = _mm_rsqrt_ss(len_sq);
	//	rlen = _mm_shuffle_ps(rlen, rlen, _MM_SHUFFLE(0, 0, 0, 0));
	//	return _mm_mul_ps(vec, rlen);
	//}

	inline __m128 _vec128_cross(const __m128& lhs, const __m128& rhs)
	{
		__m128 tmp0 = _mm_shuffle_ps(lhs, lhs, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 tmp1 = _mm_shuffle_ps(rhs, rhs, _MM_SHUFFLE(3, 1, 0, 2));
		__m128 tmp2 = _mm_mul_ps(tmp0, rhs);
		__m128 tmp3 = _mm_mul_ps(tmp0, tmp1);
		__m128 tmp4 = _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));
		return _mm_sub_ps(tmp3, tmp4);
	}


	//inline __m128 _mul_vec_to_mat(const __m128& vec, const __m128& r0, const __m128& r1, const __m128& r2)
	//{
	//	__m128 _res = _mm_setzero_ps();
	//	_res = _mm_add_ps(_res, _mm_mul_ps(vec, r0));
	//	_res = _mm_add_ps(_res, _mm_mul_ps(vec, r1));
	//	_res = _mm_add_ps(_res, _mm_mul_ps(vec, r2));
	//	return _res;
	//}

	//inline float ray_aabb_intersect_impl_sse(const __m128& ray_origin, const __m128& inv_ray_dir, const __m128& bmin, const __m128& bmax, float distance) noexcept
	//{
	//	static __m128 mask = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_set_ps(1.f, 0.f, 0.f, 0.f));
	//
	//	__m128 t1 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(bmin, mask), ray_origin), inv_ray_dir);
	//	__m128 t2 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(bmax, mask), ray_origin), inv_ray_dir);
	//
	//	__m128 vmax = _mm_max_ps(t1, t2);
	//	__m128 vmin = _mm_min_ps(t1, t2);
	//
	//	float tmax = glm::min(vmax.m128_f32[0], glm::min(vmax.m128_f32[1], vmax.m128_f32[2]));
	//	float tmin = glm::max(vmin.m128_f32[0], glm::max(vmin.m128_f32[1], vmin.m128_f32[2]));
	//
	//	if (tmax >= tmin && tmin < distance && tmax > 0.f)
	//		return tmin;
	//
	//	return std::numeric_limits<float>::max();
	//}

	inline float ray_aabb_intersect(const glm::vec3& r0, const glm::vec3& ird, const glm::vec3& bmin, const glm::vec3& bmax, float distance) noexcept
	{
		glm::vec3 vt1 = (bmin - r0) * ird;
		glm::vec3 vt2 = (bmax - r0) * ird;

		float tmin = glm::max(glm::max(glm::min(vt1.x, vt2.x), glm::min(vt1.y, vt2.y)), glm::min(vt1.z, vt2.z));
		float tmax = glm::min(glm::min(glm::max(vt1.x, vt2.x), glm::max(vt1.y, vt2.y)), glm::max(vt1.z, vt2.z));

		if (tmax >= tmin && tmin < distance && tmax > 0.f)
			return tmin;

		return std::numeric_limits<float>::max();
	}

	// Triangle intersect
	inline bool ray_triangle_intersect(const glm::vec3& r0, const glm::vec3& rd, const glm::vec3& e0, const glm::vec3& e1, const glm::vec3& v0, float& distance, glm::vec3& barycentric)
	{
		auto pvec = glm::cross(rd, e1);
		auto det = glm::dot(e0, pvec);

		//if (det < std::numeric_limits<float>::epsilon())
		//	return false;

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
}