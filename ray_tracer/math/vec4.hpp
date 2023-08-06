#pragma once

#include "math_template.hpp"

namespace math
{
	struct vec4
	{
		vec4() : x{ 0.f }, y{ 0.f }, z{ 0.f }, w{ 0.f } {}
		vec4(float v) : x{ v }, y{ v }, z{ v }, w{ v } {}
		vec4(float _x, float _y, float _z, float _w) : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}

		vec4& operator+=(const vec4& other) noexcept;
		vec4& operator-=(const vec4& other) noexcept;
		vec4& operator*=(const vec4& other) noexcept;
		vec4& operator/=(const vec4& other);

		vec4& operator+=(const float& other) noexcept;
		vec4& operator-=(const float& other) noexcept;
		vec4& operator*=(const float& other) noexcept;
		vec4& operator/=(const float& other);

		vec4 operator-() noexcept;

		const float& operator[](size_t index) const
		{
			if (index == 0ull)
				return x;
			else if (index == 1ull)
				return y;
			else if (index == 2ull)
				return z;
			else if (index == 3ull)
				return w;

			assert(false && "Index out of range");
		}

		float& operator[](size_t index)
		{
			if (index == 0ull)
				return x;
			else if (index == 1ull)
				return y;
			else if (index == 2ull)
				return z;
			else if (index == 3ull)
				return w;

			assert(false && "Index out of range");
		}

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};
#if defined(USE_INTRINSICS)
			__m128 vec128;
#else
			float data[4ull];
#endif
		};
	};


	constexpr inline bool operator==(const vec4& lhs, const vec4& rhs) noexcept
	{
		return compare_float(lhs.x, rhs.x) && compare_float(lhs.y, rhs.y) && compare_float(lhs.z, rhs.z) && compare_float(lhs.w, rhs.w);
	}

	constexpr inline bool operator!=(const vec4& lhs, const vec4& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	constexpr inline bool operator<=(const vec4& lhs, const vec4& rhs) noexcept
	{
		return greater_equal_float(lhs.x, rhs.x) && greater_equal_float(lhs.y, rhs.y) && greater_equal_float(lhs.z, rhs.z) && greater_equal_float(lhs.w, rhs.w);
	}

	constexpr inline bool operator>=(const vec4& lhs, const vec4& rhs) noexcept
	{
		return less_equal_float(lhs.x, rhs.x) && less_equal_float(lhs.y, rhs.y) && less_equal_float(lhs.z, rhs.z) && less_equal_float(lhs.w, rhs.w);
	}

	// vec to vec
	inline vec4 operator+(const vec4& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_add_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x + rhs.x;
		res.y = lhs.y + rhs.y;
		res.z = lhs.z + rhs.z;
		res.w = lhs.w + rhs.w;
#endif
		return res;
	}

	inline vec4 operator-(const vec4& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_sub_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x - rhs.x;
		res.y = lhs.y - rhs.y;
		res.z = lhs.z - rhs.z;
		res.w = lhs.w - rhs.w;
#endif
		return res;
	}

	inline vec4 operator*(const vec4& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_mul_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x * rhs.x;
		res.y = lhs.y * rhs.y;
		res.z = lhs.z * rhs.z;
		res.w = lhs.w * rhs.w;
#endif
		return res;
	}

	inline vec4 operator/(const vec4& lhs, const vec4& rhs)
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_div_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x / rhs.x;
		res.y = lhs.y / rhs.y;
		res.z = lhs.z / rhs.z;
		res.w = lhs.w / rhs.w;
#endif
		return res;
	}

	// vec to scalar
	inline vec4 operator+(const vec4& lhs, const float& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_add_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x + rhs;
		res.y = lhs.y + rhs;
		res.z = lhs.z + rhs;
		res.w = lhs.w + rhs;
#endif
		return res;
	}

	inline vec4 operator-(const vec4& lhs, const float& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_sub_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x - rhs;
		res.y = lhs.y - rhs;
		res.z = lhs.z - rhs;
		res.w = lhs.w - rhs;
#endif
		return res;
	}

	inline vec4 operator*(const vec4& lhs, const float& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_mul_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x * rhs;
		res.y = lhs.y * rhs;
		res.z = lhs.z * rhs;
		res.w = lhs.w * rhs;
#endif
		return res;
	}

	inline vec4 operator/(const vec4& lhs, const float& rhs)
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_div_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x / rhs;
		res.y = lhs.y / rhs;
		res.z = lhs.z / rhs;
		res.w = lhs.w / rhs;
#endif
		return res;
	}

	// scalar to vec
	inline vec4 operator+(const float& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_add_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs + rhs.x;
		res.y = lhs + rhs.y;
		res.z = lhs + rhs.z;
		res.w = lhs + rhs.w;
#endif
		return res;
	}

	inline vec4 operator-(const float& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_sub_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs - rhs.x;
		res.y = lhs - rhs.y;
		res.z = lhs - rhs.z;
		res.w = lhs - rhs.w;
#endif
		return res;
	}

	inline vec4 operator*(const float& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_mul_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs * rhs.x;
		res.y = lhs * rhs.y;
		res.z = lhs * rhs.z;
		res.w = lhs * rhs.w;
#endif
		return res;
	}

	inline vec4 operator/(const float& lhs, const vec4& rhs)
	{
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_div_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs / rhs.x;
		res.y = lhs / rhs.y;
		res.z = lhs / rhs.z;
		res.w = lhs / rhs.w;
#endif
		return res;
	}

	// operators
	inline vec4& vec4::operator+=(const vec4& other) noexcept
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_add_ps(vec128, other.vec128);
#else
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
#endif
		return *this;
	}

	inline vec4& vec4::operator-=(const vec4& other) noexcept
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_sub_ps(vec128, other.vec128);
#else
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
#endif
		return *this;
	}

	inline vec4& vec4::operator*=(const vec4& other) noexcept
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_mul_ps(vec128, other.vec128);
#else
		x *= other.x;
		y *= other.y;
		z *= other.z;
		w *= other.w;
#endif
		return *this;
	}

	inline vec4& vec4::operator/=(const vec4& other)
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_div_ps(vec128, other.vec128);
#else
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
#endif
		return *this;
	}

	inline vec4& vec4::operator+=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_add_ps(vec128, _mm_set_ps1(other));
#else
		x += other;
		y += other;
		z += other;
		w += other;
#endif
		return *this;
	}

	inline vec4& vec4::operator-=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_sub_ps(vec128, _mm_set_ps1(other));
#else
		x -= other;
		y -= other;
		z -= other;
		w -= other;
#endif
		return *this;
	}

	inline vec4& vec4::operator*=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_mul_ps(vec128, _mm_set_ps1(other));
#else
		x *= other;
		y *= other;
		z *= other;
		w *= other;
#endif
		return *this;
	}

	inline vec4& vec4::operator/=(const float& other)
	{
#if defined(USE_INTRINSICS)
		vec128 = _mm_div_ps(vec128, _mm_set_ps1(other));
#else
		x /= other;
		y /= other;
		z /= other;
		w /= other;
#endif
		return *this;
	}

	inline vec4 vec4::operator-() noexcept
	{
		static __m128 zero = _mm_setzero_ps();
		vec4 res{};
#if defined(USE_INTRINSICS)
		res.vec128 = _mm_sub_ps(zero, vec128);
#else
		res.x *= -1.f;
		res.y *= -1.f;
		res.z *= -1.f;
		res.w *= -1.f;
#endif
		return res;
	}

	inline vec4 make_vec4(const float* vec)
	{
		return vec4(vec[0], vec[1], vec[2], vec[3]);
	}

	inline vec4 make_vec4(const double* vec)
	{
		return vec4(vec[0], vec[1], vec[2], vec[3]);
	}
}