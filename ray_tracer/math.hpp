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

	inline float dot(const glm::vec3& a, const glm::vec3& b)
	{
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline glm::vec3 normalize(const glm::vec3& vec)
	{
		return vec / static_cast<float>(fsqrt(dot(vec, vec)));
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

	inline __m128 _mul_vec_to_mat(const __m128& vec, const __m128& r0, const __m128& r1, const __m128& r2)
	{
		__m128 _res = _mm_setzero_ps();
		_res = _mm_add_ps(_res, _mm_mul_ps(vec, r0));
		_res = _mm_add_ps(_res, _mm_mul_ps(vec, r1));
		_res = _mm_add_ps(_res, _mm_mul_ps(vec, r2));
		return _res;
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

	inline float ray_aabb_intersect_test(const glm::vec3& r0, const glm::vec3& ird, const glm::vec3& bmin, const glm::vec3& bmax, float distance)
	{
		__m128 _r0 = _mm_setr_ps(r0.x, r0.y, r0.z, 0.f);
		__m128 _ird = _mm_setr_ps(ird.x, ird.y, ird.z, 0.f);
		__m128 _bmin = _mm_setr_ps(bmin.x, bmin.y, bmin.z, 0.f);
		__m128 _bmax = _mm_setr_ps(bmax.x, bmax.y, bmax.z, 0.f);

		__m128 t1 = _mm_mul_ps(_mm_sub_ps(_bmin, _r0), _ird);
		__m128 t2 = _mm_mul_ps(_mm_sub_ps(_bmax, _r0), _ird);
		
		float vmax[4ull], vmin[4ull];
		_mm_store_ps(vmax, _mm_max_ps(t1, t2));
		_mm_store_ps(vmin, _mm_min_ps(t1, t2));
		
		float tmax = glm::min(vmax[0], glm::min(vmax[1], vmax[2]));
		float tmin = glm::max(vmin[0], glm::max(vmin[1], vmin[2]));
		
		if (tmax >= tmin && tmin < distance && tmax > 0.f)
			return tmin;
		
		return std::numeric_limits<float>::max();
	}

	// Triangle intersect

	inline bool ray_triangle_intersect_test(const glm::vec3& r0, const glm::vec3& rd, const glm::vec3& n0, const float& d0, const glm::vec3& n1, const float& d1, const glm::vec3& n2, const float& d2, float& distance, glm::vec3& barycentric)
	{
		// intersection
		//float det = glm::dot(n0, rd);
		//float dett = d0 - glm::dot(r0, n0);
		//
		//auto wr = (r0 * det) + (rd * dett);
		//barycentric.y = glm::dot(wr, n1) + det * d1;
		//barycentric.z = glm::dot(wr, n2) + det * d2;
		//
		//float tmpdet0 = det - barycentric.y - barycentric.z;
		//
		//int pdet0 = (int)tmpdet0;
		//int pdetu = (int)barycentric.y;
		//int pdetv = (int)barycentric.z;
		//
		//pdet0 = pdet0 ^ pdetu;
		//pdet0 = pdet0 | (pdetu ^ pdetv);
		//if (pdet0 & 0x80000000)
		//	return false;
		//
		//float rdet = 1.f / det;
		//barycentric.y *= rdet;
		//barycentric.z *= rdet;
		//barycentric.x = 1.f - barycentric.y - barycentric.z;
		//distance = dett * rdet;
		//
		//return true;
		static const __m128 int_coef = _mm_setr_ps(-1.f, -1.f, -1.f, 1.f);
		
		const __m128 o = _mm_setr_ps(r0.x, r0.y, r0.z, 0.f);
		const __m128 d = _mm_setr_ps(rd.x, rd.y, rd.z, 0.f);
		const __m128 n = _mm_setr_ps(n0.x, n0.y, n0.z, d0);
		
		const __m128 det = _mm_dp_ps(n, d, 0x7f);
		const __m128 dett = _mm_dp_ps(_mm_mul_ps(int_coef, n), o, 0xff);
		
		const __m128 oldt = _mm_load_ss(&distance);
		
		if ((_mm_movemask_ps(_mm_xor_ps(dett, _mm_sub_ss(_mm_mul_ss(oldt, det), dett))) & 1) == 0)
		{
			const __m128 detp = _mm_add_ps(_mm_mul_ps(o, det), _mm_mul_ps(dett, d));
			const __m128 detu = _mm_dp_ps(detp, _mm_setr_ps(n1.x, n1.y, n1.z, d1), 0xf1);
			if ((_mm_movemask_ps(_mm_xor_ps(detu, _mm_sub_ss(det, detu))) & 1) == 0)
			{
				const __m128 detv = _mm_dp_ps(detp, _mm_setr_ps(n2.x, n2.y, n2.z, d2), 0xf1);
				if ((_mm_movemask_ps(_mm_xor_ps(detv, _mm_sub_ss(det, _mm_add_ss(detu, detv)))) & 1) == 0)
				{
					const __m128 inv_det = _mm_div_ss(_mm_set_ps1(1.f), det);
					_mm_store_ss(&distance, _mm_mul_ss(dett, inv_det));
					_mm_store_ss(&barycentric.y, _mm_mul_ss(detu, inv_det));
					_mm_store_ss(&barycentric.z, _mm_mul_ss(detv, inv_det));
					barycentric.x = 1.f - barycentric.y - barycentric.z;
					//_mm_store_ps(&h.px, _mm_mul_ps(detp, _mm_shuffle_ps(inv_det, inv_det, 0)));
					return true;
				}
			}
		}
		return false;
	}

	inline bool ray_triangle_intersect(const __m128& r0, const __m128& rd, const __m128& v0, const __m128& v1, const __m128& v2, float& distance, __m128& bx, __m128& by, __m128& bz)
	{
		__m128 e0 = _mm_sub_ps(v1, v0);
		__m128 e1 = _mm_sub_ps(v2, v0);

		__m128 pvec = _vec128_cross(rd, e1);
		__m128 det = _vec128_dot_product(e0, pvec);

		if (_mm_cvtss_f32(det) < std::numeric_limits<float>::epsilon())
			return false;

		__m128 inv_det = _mm_div_ss(_mm_set_ps1(1.f), det);

		__m128 tvec = _mm_sub_ps(r0, v0);
		by = _mm_mul_ss(_vec128_dot_product(tvec, pvec), inv_det);

		auto bcy = _mm_cvtss_f32(by);
		if (bcy < 0.f || bcy > 1.f)
			return false;

		__m128 qvec = _vec128_cross(tvec, e0);
		bz = _mm_mul_ss(_vec128_dot_product(rd, qvec), inv_det);

		if (auto bcz = _mm_cvtss_f32(bz); bcz < 0.f || bcy + bcz > 1.f)
			return false;

		by = _mm_shuffle_ps(by, by, _MM_SHUFFLE(0, 0, 0, 0));
		bz = _mm_shuffle_ps(bz, bz, _MM_SHUFFLE(0, 0, 0, 0));
		bx = _mm_sub_ps(_mm_setr_ps(1.f, 1.f, 1.f, 1.f), _mm_sub_ps(by, bz));

		_mm_store_ss(&distance, _mm_mul_ss(_vec128_dot_product(e1, qvec), inv_det));

		return true;
	}

	inline bool ray_triangle_intersect(const glm::vec3& r0, const glm::vec3& rd, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& distance, glm::vec3& barycentric)
	{
		auto e0 = v1 - v0;
		auto e1 = v2 - v0;

		auto pvec = glm::cross(rd, e1);
		auto det = dot(e0, pvec);

		if (det < std::numeric_limits<float>::epsilon())
			return false;

		auto inv_det = 1.f / det;

		auto tvec = r0 - v0;
		barycentric.y = dot(tvec, pvec) * inv_det;

		if (barycentric.y < 0.f || barycentric.y > 1.f)
			return false;

		auto qvec = glm::cross(tvec, e0);
		barycentric.z = dot(rd, qvec) * inv_det;

		if (barycentric.z < 0.f || barycentric.y + barycentric.z > 1.f)
			return false;

		barycentric.x = 1.f - barycentric.y - barycentric.z;

		distance = dot(e1, qvec) * inv_det;

		return true;
	}
}