#pragma once

#include "vec3.hpp"
#include "share.hpp"

namespace math
{
	template<class _Ty>
	inline vec3<_Ty> min(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		res.x = min(lhs.x, rhs.x);
		res.y = min(lhs.y, rhs.y);
		res.z = min(lhs.z, rhs.z);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> max(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		res.x = max(lhs.x, rhs.x);
		res.y = max(lhs.y, rhs.y);
		res.z = max(lhs.z, rhs.z);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> sqrt(vec3<_Ty>& vec)
	{
		vec3<_Ty> res{};
		res.x = sqrt(vec.x);
		res.y = sqrt(vec.y);
		res.z = sqrt(vec.z);
		return res;
	}

	template<class _Ty>
	constexpr inline _Ty dot(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		_Ty res{ static_cast<_Ty>(0) };
		_vector_dot_product<_Ty, 3ull>(lhs.data, rhs.data, res);
		return res;
	}

	template<class _Ty>
	constexpr inline _Ty length2(const vec3<_Ty>& vec) noexcept
	{
		return dot(vec, vec);
	}

	template<class _Ty>
	inline _Ty length(const vec3<_Ty>& vec) noexcept
	{
		return sqrt(length2(vec));
	}

	template<class _Ty>
	inline vec3<_Ty> normalize(const vec3<_Ty>& vec)
	{
		vec3<_Ty> res{};
		res = vec / length(vec);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> apply_otb(const vec3<_Ty>& t, const vec3<_Ty>& b, const vec3<_Ty>& n, const vec3<_Ty>& v)
	{
		vec3<_Ty> res{};
		res = v.x * t + v.y * b + v.z * n;
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> reflect(const vec3<_Ty>& v, const vec3<_Ty>& n)
	{
		return v - (n * static_cast<_Ty>(2) * dot(v, n));
	}

	template<class _Ty>
	inline vec3<_Ty> refract(const vec3<_Ty>& uv, const vec3<_Ty>& n, const _Ty ior)
	{
		_Ty a = static_cast<_Ty>(0) - uv;
		_Ty cos_theta = min(dot(a, n), static_cast<_Ty>(1));
		a = cos_theta * n;
		a = uv + a;
		a = ior * a;

		_Ty b = -(static_cast<_Ty>(1) / sqrt(abs(static_cast<_Ty>(1) - length2(a))));
		_Ty c = n / b;
		return a / c;
	}

	template<class _Ty>
	inline vec3<_Ty> cross(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs)
	{
		vec3<_Ty> res{};
		res.x = lhs.y * rhs.z - lhs.z * rhs.y;
		res.y = -(lhs.x * rhs.z - lhs.z * rhs.x);
		res.z = lhs.x * rhs.y - lhs.y * rhs.x;
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> mix(const vec3<_Ty>& a, const vec3<_Ty>& b, _Ty t)
	{
		return a * (static_cast<_Ty>(1) - t) + b * t;
	}

	template<class _Ty>
	inline float ray_aabb_intersect(const vec3<_Ty>& r0, const vec3<_Ty>& ird, const vec3<_Ty>& bmin, const vec3<_Ty>& bmax, _Ty distance)
	{
		vec3<_Ty> vt1 = (bmin - r0) * ird;
		vec3<_Ty> vt2 = (bmax - r0) * ird;

		_Ty tmin = max(max(min(vt1.x, vt2.x), min(vt1.y, vt2.y)), min(vt1.z, vt2.z));
		_Ty tmax = min(min(max(vt1.x, vt2.x), max(vt1.y, vt2.y)), max(vt1.z, vt2.z));

		if (greather_equal(tmax, tmin) && tmin < distance && tmax > static_cast<_Ty>(0))
			return tmin;

		return std::numeric_limits<_Ty>::max();
	}

	template<class _Ty>
	inline bool ray_triangle_intersect(const vec3<_Ty>& r0, const vec3<_Ty>& rd, const vec3<_Ty>& v0, const vec3<_Ty>& v1, const vec3<_Ty>& v2, _Ty& distance, vec3<_Ty>& barycentric)
	{
		vec3<_Ty> e0 = v1 - v0;
		vec3<_Ty> e1 = v2 - v0;

		vec3<_Ty> pvec = cross(rd, e1);
		_Ty det = dot(e0, pvec);

		if (det < std::numeric_limits<_Ty>::epsilon())
			return false;

		_Ty inv_det = static_cast<_Ty>(1) / det;

		vec3<_Ty> tvec = r0 - v0;
		barycentric.y = dot(tvec, pvec) * inv_det;

		if (barycentric.y < static_cast<_Ty>(0) || barycentric.y > static_cast<_Ty>(1))
			return false;

		vec3<_Ty> qvec = cross(tvec, e0);
		barycentric.z = dot(rd, qvec) * inv_det;

		if (barycentric.z < static_cast<_Ty>(0) || barycentric.y + barycentric.z > static_cast<_Ty>(1))
			return false;

		barycentric.x = static_cast<_Ty>(1) - barycentric.y - barycentric.z;

		distance = dot(e1, qvec) * inv_det;

		return true;
	}

	template<class _Ty>
	inline vec3<_Ty> interpolate(const vec3<_Ty>& v0, const vec3<_Ty>& v1, const vec3<_Ty>& v2, const vec3<_Ty>& interpolator)
	{
		vec3<_Ty> res{};
		res = interpolator.x * v0 + interpolator.y * v1 + interpolator.z * v2;
		return res;
	}
}