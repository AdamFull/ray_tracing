#pragma once

#include "vec3.hpp"
#include "share.hpp"

namespace math
{
	template<>
	inline vec3 min(const vec3& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_min_ps(lhs.vec128, rhs.vec128);
#else
		res.x = std::min(lhs.x, rhs.x);
		res.y = std::min(lhs.y, rhs.y);
		res.z = std::min(lhs.z, rhs.z);
#endif
		return res;
	}

	template<>
	inline vec3 max(const vec3& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_max_ps(lhs.vec128, rhs.vec128);
#else
		res.x = std::max(lhs.x, rhs.x);
		res.y = std::max(lhs.y, rhs.y);
		res.z = std::max(lhs.z, rhs.z);
#endif
		return res;
	}

	template<>
	inline vec3 sqrt(vec3& vec)
	{
		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_sqrt_ps(vec.vec128);
#else
		res.x = std::sqrt(vec.x);
		res.y = std::sqrt(vec.y);
		res.z = std::sqrt(vec.z);
#endif
		return res;
	}

	inline float dot(const vec3& lhs, const vec3& rhs)
	{
#if defined(USE_INTRINSICS)
		return _mm_cvtss_f32(_vec128_dot_product(lhs.vec128, rhs.vec128));
#else
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
#endif
	}

	inline float length2(const vec3& vec) noexcept
	{
		return dot(vec, vec);
	}

	inline float length(const vec3& vec) noexcept
	{
#if defined(USE_INTRINSICS)
		return _mm_cvtss_f32(math::_vec128_length(vec.vec128));
#else
		return std::sqrt(length2(vec));
#endif
	}

	inline vec3 normalize(const vec3& vec)
	{
		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = math::_vec128_normalize(vec.vec128);
#else
		auto len = length(vec);
		res = vec / len;
#endif
		return res;
	}

	inline vec3 apply_otb(const vec3& t, const vec3& b, const vec3& n, const vec3& v)
	{
#if defined(USE_INTRINSICS)
		__m128 x = _mm_mul_ps(_mm_shuffle_ps(v.vec128, v.vec128, _MM_SHUFFLE(0, 0, 0, 0)), t.vec128);
		__m128 y = _mm_mul_ps(_mm_shuffle_ps(v.vec128, v.vec128, _MM_SHUFFLE(1, 1, 1, 1)), b.vec128);
		__m128 z = _mm_mul_ps(_mm_shuffle_ps(v.vec128, v.vec128, _MM_SHUFFLE(2, 2, 2, 2)), n.vec128);
#endif

		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_add_ps(x, _mm_add_ps(y, z));
#else
		res = v.x * t + v.y * b + v.z * n;
#endif
		return res;
	}

	inline vec3 reflect(const vec3& v, const vec3& n)
	{
		//static __m128 cv2 = _mm_set_ps1(2.f);
		//
		//vec3 res{};
		//__m128 dp = _vec128_dot_product(v.vec128, n.vec128);
		//dp = _mm_shuffle_ps(dp, dp, _MM_SHUFFLE(0, 0, 0, 0));
		//__m128 n2 = _mm_mul_ps(n.vec128, cv2);
		//res.vec128 = _mm_mul_ps(n2, dp);
		//res.vec128 = _mm_sub_ps(v.vec128, res.vec128);
		//return res;
		return v - (n * 2.f * dot(v, n));
	}

	inline vec3 refract(const vec3& uv, const vec3& n, const float ior)
	{
		auto a = 0.f - uv;
		auto cos_theta = std::min(dot(a, n), 1.f);
		a = cos_theta * n;
		a = uv + a;
		a = ior * a;

		auto b = -(1.f / std::sqrt(std::fabs(1.f - length2(a))));
		auto c = n / b;
		return a / c;
	}

	inline vec3 cross(const vec3& lhs, const vec3& rhs)
	{
		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _vec128_cross(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.y * rhs.z - lhs.z * rhs.y;
		res.y = -(lhs.x * rhs.z - lhs.z * rhs.x);
		res.z = lhs.x * rhs.y - lhs.y * rhs.x;
#endif
		return res;
	}

	inline vec3 mix(const vec3& a, const vec3& b, float t)
	{
		return a * (1.f - t) + b * t;
	}


	inline bool ray_aabb_intersect(const vec3& ray_origin, const vec3& inv_ray_dir, const vec3& bmin, const vec3& bmax)
	{
#if defined(USE_INTRINSICS)
		static __m128 mask = _mm_cmpeq_ps(_mm_setzero_ps(), _mm_set_ps(1.f, 0.f, 0.f, 0.f));

		__m128 t1 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(bmin.vec128, mask), ray_origin.vec128), inv_ray_dir.vec128);
		__m128 t2 = _mm_mul_ps(_mm_sub_ps(_mm_and_ps(bmax.vec128, mask), ray_origin.vec128), inv_ray_dir.vec128);

		__m128 vmax = _mm_max_ps(t1, t2);
		__m128 vmin = _mm_min_ps(t1, t2);

		float tmax = std::min(vmax.m128_f32[0], std::min(vmax.m128_f32[1], vmax.m128_f32[2]));
		float tmin = std::max(vmin.m128_f32[0], std::max(vmin.m128_f32[1], vmin.m128_f32[2]));
#else
		float tx1 = (bmin.x - ray_origin.x) * inv_ray_dir.x, tx2 = (bmax.x - ray_origin.x) * inv_ray_dir.x;
		float tmin = min(tx1, tx2), tmax = max(tx1, tx2);
		float ty1 = (bmin.y - ray_origin.y) * inv_ray_dir.y, ty2 = (bmax.y - ray_origin.y) * inv_ray_dir.y;
		tmin = max(tmin, min(ty1, ty2)), tmax = min(tmax, max(ty1, ty2));
		float tz1 = (bmin.z - ray_origin.z) * inv_ray_dir.z, tz2 = (bmax.z - ray_origin.z) * inv_ray_dir.z;
		tmin = max(tmin, min(tz1, tz2)), tmax = min(tmax, max(tz1, tz2));
#endif

		return tmin <= tmax && tmax >= 0.f;
	}

	inline bool ray_triangle_intersect(const vec3& r0, const vec3& rd, const vec3& v0, const vec3& v1, const vec3& v2, float& distance, vec3& barycentric)
	{
#if defined(USE_INTRINSICS)
		__m128 e0 = _mm_sub_ps(v1.vec128, v0.vec128);
		__m128 e1 = _mm_sub_ps(v2.vec128, v0.vec128);

		__m128 pvec =_vec128_cross(rd.vec128, e1);
		__m128 det = _vec128_dot_product(e0, pvec);

		if (_mm_cvtss_f32(det) < std::numeric_limits<float>::epsilon())
			return false;

		__m128 inv_det = _mm_div_ss(_mm_set_ps1(1.f), det);

		__m128 tvec = _mm_sub_ps(r0.vec128, v0.vec128);
		barycentric.y = _mm_cvtss_f32(_mm_mul_ss(_vec128_dot_product(tvec, pvec), inv_det));

		if (barycentric.y < 0.f || barycentric.y > 1.f)
			return false;

		__m128 qvec = _vec128_cross(tvec, e0);
		barycentric.z = _mm_cvtss_f32(_mm_mul_ss(_vec128_dot_product(rd.vec128, qvec), inv_det));

		if (barycentric.z < 0.f || barycentric.y + barycentric.z > 1.f)
			return false;

		barycentric.x = 1.f - barycentric.y - barycentric.z;

		distance = _mm_cvtss_f32(_mm_mul_ss(_vec128_dot_product(e1, qvec), inv_det));
#else
		auto e0 = v1 - v0;
		auto e1 = v2 - v0;

		auto pvec = cross(rd, e1);
		auto det = dot(e0, pvec);

		if (det < std::numeric_limits<float>::epsilon())
			return false;

		auto inv_det = 1.f / det;

		auto tvec = r0 - v0;
		barycentric.y = dot(tvec, pvec) * inv_det;

		if (barycentric.y < 0.f || barycentric.y > 1.f)
			return false;

		auto qvec = cross(tvec, e0);
		barycentric.z = dot(rd, qvec) * inv_det;

		if (barycentric.z < 0.f || barycentric.y + barycentric.z > 1.f)
			return false;

		barycentric.x = 1.f - barycentric.y - barycentric.z;

		distance = dot(e1, qvec) * inv_det;
#endif

		return true;
	}

	inline vec3 interpolate(const vec3& v0, const vec3& v1, const vec3& v2, const vec3& interpolator)
	{
#if defined(USE_INTRINSICS)
		__m128 interpolator_x = _mm_shuffle_ps(interpolator.vec128, interpolator.vec128, _MM_SHUFFLE(0, 0, 0, 0));
		__m128 interpolator_y = _mm_shuffle_ps(interpolator.vec128, interpolator.vec128, _MM_SHUFFLE(1, 1, 1, 1));
		__m128 interpolator_z = _mm_shuffle_ps(interpolator.vec128, interpolator.vec128, _MM_SHUFFLE(2, 2, 2, 2));
#endif

		vec3 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_add_ps(_mm_mul_ps(interpolator_x, v0.vec128), _mm_add_ps(_mm_mul_ps(interpolator_y, v1.vec128), _mm_mul_ps(interpolator_z, v2.vec128)));
#else
		res = interpolator.x* v0 + interpolator.y * v1 + interpolator.z * v2;
#endif
		return res;
	}
}