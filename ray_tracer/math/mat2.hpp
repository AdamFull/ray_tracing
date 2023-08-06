#pragma once

#include "vec2_math.hpp"

namespace math
{
	struct mat2
	{
		mat2()
		{
			data[0ull].x = 1.f;
			data[1ull].y = 1.f;
		}

		mat2(float v)
		{
			data[0ull].x = v;
			data[1ull].y = v;
		}

		mat2(
			float x0, float y0,
			float x1, float y1)
		{
			data[0ull].x = x0;
			data[0ull].y = y0;

			data[1ull].x = x1;
			data[1ull].y = y1;
		}

		mat2(const vec2& v0, const vec2& v1, const vec2& v2)
		{
			data[0ull] = v0;
			data[1ull] = v1;
		}

		mat2& operator+=(const mat2& other) noexcept;
		mat2& operator-=(const mat2& other) noexcept;
		mat2& operator*=(const mat2& other) noexcept;

		mat2& operator+=(const vec2& other) noexcept;
		mat2& operator-=(const vec2& other) noexcept;
		mat2& operator*=(const vec2& other) noexcept;

		mat2& operator+=(const float& other) noexcept;
		mat2& operator-=(const float& other) noexcept;
		mat2& operator*=(const float& other) noexcept;

		const vec2& operator[](size_t index) const
		{
			return data[index];
		}

		vec2& operator[](size_t index)
		{
			return data[index];
		}

		vec2 data[2ull];
	};

	constexpr inline bool operator==(const mat2& lhs, const mat2& rhs) noexcept
	{
		return lhs[0ull] == rhs[0ull] && lhs[1ull] == rhs[1ull];
	}

	constexpr inline bool operator!=(const mat2& lhs, const mat2& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	constexpr inline bool operator<=(const mat2& lhs, const mat2& rhs) noexcept
	{
		return lhs[0ull] >= rhs[0ull] && lhs[1ull] >= rhs[1ull];
	}

	constexpr inline bool operator>=(const mat2& lhs, const mat2& rhs) noexcept
	{
		return lhs[0ull] <= rhs[0ull] && lhs[1ull] <= rhs[1ull];
	}

	// mat to mat
	inline mat2 operator+(const mat2& lhs, const mat2& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] + rhs[0ull];
		res[1ull] = lhs[1ull] + rhs[1ull];
		return res;
	}

	inline mat2 operator-(const mat2& lhs, const mat2& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] - rhs[0ull];
		res[1ull] = lhs[1ull] - rhs[1ull];
		return res;
	}

	inline mat2 operator*(const mat2& lhs, const mat2& rhs) noexcept
	{
		mat2 res{ 0.f };
		for (size_t r = 0ull; r < 2ull; ++r)
		{
			for (size_t c = 0ull; c < 2ull; ++c)
			{
				res[r][c] = 0.f;
				for (size_t k = 0ull; k < 2ull; ++k)
					res[r][c] += lhs[r][k] * rhs[k][c];
			}
		}
		return res;
	}

	// mat to vector
	inline mat2 operator+(const mat2& lhs, const vec2& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] + rhs;
		res[1ull] = lhs[1ull] + rhs;
		return res;
	}

	inline mat2 operator-(const mat2& lhs, const vec2& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] - rhs;
		res[1ull] = lhs[1ull] - rhs;
		return res;
	}

	inline mat2 operator*(const mat2& lhs, const vec2& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] * rhs;
		res[1ull] = lhs[1ull] * rhs;
		return res;
	}

	inline vec2 operator*(const vec2& lhs, const mat2& rhs) noexcept
	{
		vec2 res{};
		res += lhs * rhs[0ull];
		res += lhs * rhs[1ull];
		return res;
	}

	// mat to scalar
	inline mat2 operator+(const mat2& lhs, const float& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] + rhs;
		res[1ull] = lhs[1ull] + rhs;
		return res;
	}

	inline mat2 operator-(const mat2& lhs, const float& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] - rhs;
		res[1ull] = lhs[1ull] - rhs;
		return res;
	}

	inline mat2 operator*(const mat2& lhs, const float& rhs) noexcept
	{
		mat2 res{};
		res[0ull] = lhs[0ull] * rhs;
		res[1ull] = lhs[1ull] * rhs;
		return res;
	}

	// operators
	mat2& mat2::operator+=(const mat2& other) noexcept
	{
		data[0ull] += other[0ull];
		data[1ull] += other[1ull];
		return *this;
	}

	mat2& mat2::operator-=(const mat2& other) noexcept
	{
		data[0ull] -= other[0ull];
		data[1ull] -= other[1ull];
		return *this;
	}

	mat2& mat2::operator*=(const mat2& other) noexcept
	{
		mat2 tmp{ *this };
		for (size_t r = 0ull; r < 2ull; ++r)
		{
			for (size_t c = 0ull; c < 2ull; ++c)
			{
				data[r][c] = 0.f;
				for (size_t k = 0ull; k < 2ull; ++k)
					data[r][c] += tmp[r][k] * other[k][c];
			}
		}
		return *this;
	}

	mat2& mat2::operator+=(const vec2& other) noexcept
	{
		data[0ull] += other;
		data[1ull] += other;
		return *this;
	}

	mat2& mat2::operator-=(const vec2& other) noexcept
	{
		data[0ull] -= other;
		data[1ull] -= other;
		return *this;
	}

	mat2& mat2::operator*=(const vec2& other) noexcept
	{
		data[0ull] *= other;
		data[1ull] *= other;
		return *this;
	}

	mat2& mat2::operator+=(const float& other) noexcept
	{
		data[0ull] += other;
		data[1ull] += other;
		return *this;
	}

	mat2& mat2::operator-=(const float& other) noexcept
	{
		data[0ull] -= other;
		data[1ull] -= other;
		return *this;
	}

	mat2& mat2::operator*=(const float& other) noexcept
	{
		data[0ull] *= other;
		data[1ull] *= other;
		return *this;
	}

	mat2 make_mat2(const float* data)
	{
		mat2 mat{};
		mat[0ull] = make_vec2(data);
		mat[1ull] = make_vec2(data + 4ull);
		return mat;
	}

	mat2 make_mat2(const double* data)
	{
		mat2 mat{};
		mat[0ull] = make_vec2(data);
		mat[1ull] = make_vec2(data + 4ull);
		return mat;
	}
}