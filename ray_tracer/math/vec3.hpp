#pragma once

#include "math_template.hpp"

namespace math
{
	struct vec3
	{
		vec3() : x{ 0.f }, y{ 0.f }, z{ 0.f }, align{ 0.f } {}
		vec3(float v) : x{ v }, y{ v }, z{ v }, align{ 0.f } {}
		vec3(float _x, float _y, float _z)
		{
			x = _x;
			y = _y;
			z = _z;
			align = 0.f;
		}

		vec3& operator+=(const vec3& other) noexcept;
		vec3& operator-=(const vec3& other) noexcept;
		vec3& operator*=(const vec3& other) noexcept;
		vec3& operator/=(const vec3& other);

		vec3& operator+=(const float& other) noexcept;
		vec3& operator-=(const float& other) noexcept;
		vec3& operator*=(const float& other) noexcept;
		vec3& operator/=(const float& other);

		vec3 operator-() noexcept;

		const float& operator[](size_t index) const
		{
			if (index == 0ull)
				return x;
			else if (index == 1ull)
				return y;
			else if (index == 2ull)
				return z;

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

			assert(false && "Index out of range");
		}

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float align;
			};
#if defined(USE_INTRINSICS)
			__m128 vec128;
#else
			float data[4ull];
#endif
		};
	};

	constexpr inline bool operator==(const vec3& lhs, const vec3& rhs) noexcept
	{
		return compare_float(lhs.x, rhs.x) && compare_float(lhs.y, rhs.y) && compare_float(lhs.z, rhs.z);
	}

	constexpr inline bool operator!=(const vec3& lhs, const vec3& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	constexpr inline bool operator<=(const vec3& lhs, const vec3& rhs) noexcept
	{
		return greater_equal_float(lhs.x, rhs.x) && greater_equal_float(lhs.y, rhs.y) && greater_equal_float(lhs.z, rhs.z);
	}

	constexpr inline bool operator>=(const vec3& lhs, const vec3& rhs) noexcept
	{
		return less_equal_float(lhs.x, rhs.x) && less_equal_float(lhs.y, rhs.y) && less_equal_float(lhs.z, rhs.z);
	}

	// vec to vec
	inline vec3 operator+(const vec3& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_add_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x + rhs.x;
		res.y = lhs.y + rhs.y;
		res.z = lhs.z + rhs.z;
#endif
		return res;
	}

	inline vec3 operator-(const vec3& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x - rhs.x;
		res.y = lhs.y - rhs.y;
		res.z = lhs.z - rhs.z;
#endif
		return res;
	}

	inline vec3 operator*(const vec3& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_mul_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x * rhs.x;
		res.y = lhs.y * rhs.y;
		res.z = lhs.z * rhs.z;
#endif
		return res;
	}

	inline vec3 operator/(const vec3& lhs, const vec3& rhs)
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_div_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x / rhs.x;
		res.y = lhs.y / rhs.y;
		res.z = lhs.z / rhs.z;
#endif
		return res;
	}

	// vec to scalar
	inline vec3 operator+(const vec3& lhs, const float& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_add_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x + rhs;
		res.y = lhs.y + rhs;
		res.z = lhs.z + rhs;
#endif
		return res;
	}

	inline vec3 operator-(const vec3& lhs, const float& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x - rhs;
		res.y = lhs.y - rhs;
		res.z = lhs.z - rhs;
#endif
		return res;
	}

	inline vec3 operator*(const vec3& lhs, const float& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_mul_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x * rhs;
		res.y = lhs.y * rhs;
		res.z = lhs.z * rhs;
#endif
		return res;
	}

	inline vec3 operator/(const vec3& lhs, const float& rhs)
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_div_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x / rhs;
		res.y = lhs.y / rhs;
		res.z = lhs.z / rhs;
#endif
		return res;
	}

	// scalar to vec
	inline vec3 operator+(const float& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_add_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs + rhs.x;
		res.y = lhs + rhs.y;
		res.z = lhs + rhs.z;
#endif
		return res;
	}

	inline vec3 operator-(const float& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs - rhs.x;
		res.y = lhs - rhs.y;
		res.z = lhs - rhs.z;
#endif
		return res;
	}

	inline vec3 operator*(const float& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_mul_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs * rhs.x;
		res.y = lhs * rhs.y;
		res.z = lhs * rhs.z;
#endif
		return res;
	}

	inline vec3 operator/(const float& lhs, const vec3& rhs)
	{
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_div_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs / rhs.x;
		res.y = lhs / rhs.y;
		res.z = lhs / rhs.z;
#endif
		return res;
	}

	// operators
	inline vec3& vec3::operator+=(const vec3& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_add_ps(vec128, other.vec128);
#else
		x += other.x;
		y += other.y;
		z += other.z;
#endif
		return *this;
	}

	inline vec3& vec3::operator-=(const vec3& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_sub_ps(vec128, other.vec128);
#else
		x -= other.x;
		y -= other.y;
		z -= other.z;
#endif
		return *this;
	}

	inline vec3& vec3::operator*=(const vec3& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_mul_ps(vec128, other.vec128);
#else
		x *= other.x;
		y *= other.y;
		z *= other.z;
#endif
		return *this;
	}

	inline vec3& vec3::operator/=(const vec3& other)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_div_ps(vec128, other.vec128);
#else
		x /= other.x;
		y /= other.y;
		z /= other.z;
#endif
		return *this;
	}

	inline vec3& vec3::operator+=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_add_ps(vec128, _mm_set_ps1(other));
#else
		x += other;
		y += other;
		z += other;
#endif
		return *this;
	}

	inline vec3& vec3::operator-=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_sub_ps(vec128, _mm_set_ps1(other));
#else
		x -= other;
		y -= other;
		z -= other;
#endif
		return *this;
	}

	inline vec3& vec3::operator*=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_mul_ps(vec128, _mm_set_ps1(other));
#else
		x *= other;
		y *= other;
		z *= other;
#endif
		return *this;
	}

	inline vec3& vec3::operator/=(const float& other)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_div_ps(vec128, _mm_set_ps1(other));
#else
		x /= other;
		y /= other;
		z /= other;
#endif
		return *this;
	}

	inline vec3 vec3::operator-() noexcept
	{
		static __m128 zero = _mm_setzero_ps();
		vec3 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(zero, vec128);
#else
		res.x *= -1.f;
		res.y *= -1.f;
		res.z *= -1.f;
#endif
		return res;
	}

	inline vec3 make_vec3(const float* vec)
	{
		return vec3(vec[0], vec[1], vec[2]);
	}

	inline vec3 make_vec3(const double* vec)
	{
		return vec3(vec[0], vec[1], vec[2]);
	}
}