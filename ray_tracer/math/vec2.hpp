#pragma once

#include "math_template.hpp"

namespace math
{
	struct vec2
	{
		vec2() : x{ 0.f }, y{ 0.f }, align0{ 0.f }, align1{ 0.f } {}
		vec2(float v) : x{ v }, y{ v }, align0{ 0.f }, align1{ 0.f } {}
		vec2(float _x, float _y) : x{ _x }, y{ _y }, align0{ 0.f }, align1{ 0.f } {}

		vec2& operator+=(const vec2& other) noexcept;
		vec2& operator-=(const vec2& other) noexcept;
		vec2& operator*=(const vec2& other) noexcept;
		vec2& operator/=(const vec2& other);

		vec2& operator+=(const float& other) noexcept;
		vec2& operator-=(const float& other) noexcept;
		vec2& operator*=(const float& other) noexcept;
		vec2& operator/=(const float& other);

		vec2 operator-() noexcept;

		const float& operator[](size_t index) const
		{
			if (index == 0ull)
				return x;
			else if (index == 1ull)
				return y;

			assert(false && "Index out of range");
		}

		float& operator[](size_t index)
		{
			if (index == 0ull)
				return x;
			else if (index == 1ull)
				return y;

			assert(false && "Index out of range");
		}

		union
		{
			struct
			{
				float x;
				float y;
				float align0;
				float align1;
			};
#if defined(USE_INTRINSICS)
			__m128 vec128;
#else
			float data[4ull];
#endif
		};
	};


	constexpr inline bool operator==(const vec2& lhs, const vec2& rhs) noexcept
	{
		return compare_float(lhs.x, rhs.x) && compare_float(lhs.y, rhs.y);
	}

	constexpr inline bool operator!=(const vec2& lhs, const vec2& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	constexpr inline bool operator<=(const vec2& lhs, const vec2& rhs) noexcept
	{
		return greater_equal_float(lhs.x, rhs.x) && greater_equal_float(lhs.y, rhs.y);
	}

	constexpr inline bool operator>=(const vec2& lhs, const vec2& rhs) noexcept
	{
		return less_equal_float(lhs.x, rhs.x) && less_equal_float(lhs.y, rhs.y);
	}

	// vec to vec
	inline vec2 operator+(const vec2& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_add_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x + rhs.x;
		res.y = lhs.y + rhs.y;
#endif
		return res;
	}

	inline vec2 operator-(const vec2& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x - rhs.x;
		res.y = lhs.y - rhs.y;
#endif
		return res;
	}

	inline vec2 operator*(const vec2& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_mul_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x * rhs.x;
		res.y = lhs.y * rhs.y;
#endif
		return res;
	}

	inline vec2 operator/(const vec2& lhs, const vec2& rhs)
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_div_ps(lhs.vec128, rhs.vec128);
#else
		res.x = lhs.x / rhs.x;
		res.y = lhs.y / rhs.y;
#endif
		return res;
	}

	// vec to scalar
	inline vec2 operator+(const vec2& lhs, const float& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_add_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x + rhs;
		res.y = lhs.y + rhs;
#endif
		return res;
	}

	inline vec2 operator-(const vec2& lhs, const float& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x - rhs;
		res.y = lhs.y - rhs;
#endif
		return res;
	}

	inline vec2 operator*(const vec2& lhs, const float& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_mul_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x * rhs;
		res.y = lhs.y * rhs;
#endif
		return res;
	}

	inline vec2 operator/(const vec2& lhs, const float& rhs)
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_div_ps(lhs.vec128, _mm_set_ps1(rhs));
#else
		res.x = lhs.x / rhs;
		res.y = lhs.y / rhs;
#endif
		return res;
	}

	// scalar to vec
	inline vec2 operator+(const float& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_add_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs + rhs.x;
		res.y = lhs + rhs.y;
#endif
		return res;
	}

	inline vec2 operator-(const float& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs - rhs.x;
		res.y = lhs - rhs.y;
#endif
		return res;
	}

	inline vec2 operator*(const float& lhs, const vec2& rhs) noexcept
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_mul_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs * rhs.x;
		res.y = lhs * rhs.y;
#endif
		return res;
	}

	inline vec2 operator/(const float& lhs, const vec2& rhs)
	{
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_div_ps(_mm_set_ps1(lhs), rhs.vec128);
#else
		res.x = lhs / rhs.x;
		res.y = lhs / rhs.y;
#endif
		return res;
	}

	// operators
	inline vec2& vec2::operator+=(const vec2& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_add_ps(vec128, other.vec128);
#else
		x += other.x;
		y += other.y;
#endif
		return *this;
	}

	inline vec2& vec2::operator-=(const vec2& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_sub_ps(vec128, other.vec128);
#else
		x -= other.x;
		y -= other.y;
#endif
		return *this;
	}

	inline vec2& vec2::operator*=(const vec2& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_mul_ps(vec128, other.vec128);
#else
		x *= other.x;
		y *= other.y;
#endif
		return *this;
	}

	inline vec2& vec2::operator/=(const vec2& other)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_div_ps(vec128, other.vec128);
#else
		x /= other.x;
		y /= other.y;
#endif
		return *this;
	}

	inline vec2& vec2::operator+=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_add_ps(vec128, _mm_set_ps1(other));
#else
		x += other;
		y += other;
#endif
		return *this;
	}

	inline vec2& vec2::operator-=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_sub_ps(vec128, _mm_set_ps1(other));
#else
		x -= other;
		y -= other;
#endif
		return *this;
	}

	inline vec2& vec2::operator*=(const float& other) noexcept
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_mul_ps(vec128, _mm_set_ps1(other));
#else
		x *= other;
		y *= other;
#endif
		return *this;
	}

	inline vec2& vec2::operator/=(const float& other)
	{
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		vec128 = _mm_div_ps(vec128, _mm_set_ps1(other));
#else
		x /= other;
		y /= other;
#endif
		return *this;
	}

	inline vec2 vec2::operator-() noexcept
	{
		static __m128 zero = _mm_setzero_ps();
		vec2 res{};
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_BASIC_ARITHMETIC)
		res.vec128 = _mm_sub_ps(zero, vec128);
#else
		res.x *= -1.f;
		res.y *= -1.f;
#endif
		return res;
	}

	inline vec2 make_vec2(const float* vec)
	{
		return vec2(vec[0], vec[1]);
	}

	inline vec2 make_vec2(const double* vec)
	{
		return vec2(vec[0], vec[1]);
	}
}