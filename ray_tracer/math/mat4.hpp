#pragma once

#include "vec4_math.hpp"

namespace math
{
	struct mat4
	{
		mat4()
		{
			data[0ull].x = 1.f;
			data[1ull].y = 1.f;
			data[2ull].z = 1.f;
			data[3ull].w = 1.f;
		}

		mat4(float v)
		{
			data[0ull].x = v;
			data[1ull].y = v;
			data[2ull].z = v;
			data[3ull].w = v;
		}

		mat4(
			float x0, float y0, float z0, float w0,
			float x1, float y1, float z1, float w1,
			float x2, float y2, float z2, float w2,
			float x3, float y3, float z3, float w3)
		{
			data[0ull].x = x0;
			data[0ull].y = y0;
			data[0ull].z = z0;
			data[0ull].w = w0;

			data[1ull].x = x1;
			data[1ull].y = y1;
			data[1ull].z = z1;
			data[1ull].w = w1;

			data[2ull].x = x2;
			data[2ull].y = y2;
			data[2ull].z = z2;
			data[2ull].w = w2;

			data[3ull].x = x3;
			data[3ull].y = y3;
			data[3ull].z = z3;
			data[3ull].w = w3;
		}

		mat4(const vec4& v0, const vec4& v1, const vec4& v2, const vec4& v3)
		{
			data[0ull] = v0;
			data[1ull] = v1;
			data[2ull] = v2;
			data[3ull] = v3;
		}

		mat4& operator+=(const mat4& other) noexcept;
		mat4& operator-=(const mat4& other) noexcept;
		mat4& operator*=(const mat4& other) noexcept;

		mat4& operator+=(const vec4& other) noexcept;
		mat4& operator-=(const vec4& other) noexcept;
		mat4& operator*=(const vec4& other) noexcept;

		mat4& operator+=(const float& other) noexcept;
		mat4& operator-=(const float& other) noexcept;
		mat4& operator*=(const float& other) noexcept;

		const vec4& operator[](size_t index) const
		{
			return data[index];
		}

		vec4& operator[](size_t index)
		{
			return data[index];
		}


		vec4 data[4ull];
	};

	constexpr inline bool operator==(const mat4& lhs, const mat4& rhs) noexcept
	{
		return lhs[0ull] == rhs[0ull] && lhs[1ull] == rhs[1ull] && lhs[2ull] == rhs[2ull] && lhs[3ull] == rhs[3ull];
	}

	constexpr inline bool operator!=(const mat4& lhs, const mat4& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	constexpr inline bool operator<=(const mat4& lhs, const mat4& rhs) noexcept
	{
		return lhs[0ull] >= rhs[0ull] && lhs[1ull] >= rhs[1ull] && lhs[2ull] >= rhs[2ull] && lhs[3ull] >= rhs[3ull];
	}

	constexpr inline bool operator>=(const mat4& lhs, const mat4& rhs) noexcept
	{
		return lhs[0ull] <= rhs[0ull] && lhs[1ull] <= rhs[1ull] && lhs[2ull] <= rhs[2ull] && lhs[3ull] <= rhs[3ull];
	}

	// mat to mat
	inline mat4 operator+(const mat4& lhs, const mat4& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] + rhs[0ull];
		res[1ull] = lhs[1ull] + rhs[1ull];
		res[2ull] = lhs[2ull] + rhs[2ull];
		res[3ull] = lhs[3ull] + rhs[3ull];
		return res;
	}

	inline mat4 operator-(const mat4& lhs, const mat4& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] - rhs[0ull];
		res[1ull] = lhs[1ull] - rhs[1ull];
		res[2ull] = lhs[2ull] - rhs[2ull];
		res[3ull] = lhs[3ull] - rhs[3ull];
		return res;
	}

	inline mat4 operator*(const mat4& lhs, const mat4& rhs) noexcept
	{
		mat4 res{ 0.f };
		for (size_t r = 0ull; r < 4ull; ++r)
		{
			for (size_t c = 0ull; c < 4ull; ++c)
			{
				res[r][c] = 0.f;
				for (size_t k = 0ull; k < 4ull; ++k)
					res[r][c] += lhs[r][k] * rhs[k][c];
			}
		}
		return res;
	}

	// mat to vector
	inline mat4 operator+(const mat4& lhs, const vec4& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] + rhs;
		res[1ull] = lhs[1ull] + rhs;
		res[2ull] = lhs[2ull] + rhs;
		res[3ull] = lhs[3ull] + rhs;
		return res;
	}

	inline mat4 operator-(const mat4& lhs, const vec4& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] - rhs;
		res[1ull] = lhs[1ull] - rhs;
		res[2ull] = lhs[2ull] - rhs;
		res[3ull] = lhs[3ull] - rhs;
		return res;
	}

	inline vec4 operator*(const mat4& lhs, const vec4& rhs) noexcept
	{
		vec4 res{};
		res += lhs[0ull] * rhs;
		res += lhs[1ull] * rhs;
		res += lhs[2ull] * rhs;
		res += lhs[3ull] * rhs;
		return res;
	}

	inline vec4 operator*(const vec4& lhs, const mat4& rhs) noexcept
	{
		vec4 res{};
		res += lhs * rhs[0ull];
		res += lhs * rhs[1ull];
		res += lhs * rhs[2ull];
		res += lhs * rhs[3ull];
		return res;
	}

	// mat to scalar
	inline mat4 operator+(const mat4& lhs, const float& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] + rhs;
		res[1ull] = lhs[1ull] + rhs;
		res[2ull] = lhs[2ull] + rhs;
		res[3ull] = lhs[3ull] + rhs;
		return res;
	}

	inline mat4 operator-(const mat4& lhs, const float& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] - rhs;
		res[1ull] = lhs[1ull] - rhs;
		res[2ull] = lhs[2ull] - rhs;
		res[3ull] = lhs[3ull] - rhs;
		return res;
	}

	inline mat4 operator*(const mat4& lhs, const float& rhs) noexcept
	{
		mat4 res{};
		res[0ull] = lhs[0ull] * rhs;
		res[1ull] = lhs[1ull] * rhs;
		res[2ull] = lhs[2ull] * rhs;
		res[3ull] = lhs[3ull] * rhs;
		return res;
	}

	// operators
	mat4& mat4::operator+=(const mat4& other) noexcept
	{
		data[0ull] += other[0ull];
		data[1ull] += other[1ull];
		data[2ull] += other[2ull];
		data[3ull] += other[3ull];
		return *this;
	}

	mat4& mat4::operator-=(const mat4 & other) noexcept
	{
		data[0ull] -= other[0ull];
		data[1ull] -= other[1ull];
		data[2ull] -= other[2ull];
		data[3ull] -= other[3ull];
		return *this;
	}

	mat4& mat4::operator*=(const mat4 & other) noexcept
	{
		mat4 tmp{ *this };
		for (size_t r = 0ull; r < 4ull; ++r)
		{
			for (size_t c = 0ull; c < 4ull; ++c)
			{
				data[r][c] = 0.f;
				for (size_t k = 0ull; k < 4ull; ++k)
					data[r][c] += tmp[r][k] * other[k][c];
			}
		}
		return *this;
	}

	mat4& mat4::operator+=(const vec4& other) noexcept
	{
		data[0ull] += other;
		data[1ull] += other;
		data[2ull] += other;
		data[3ull] += other;
		return *this;
	}

	mat4& mat4::operator-=(const vec4& other) noexcept
	{
		data[0ull] -= other;
		data[1ull] -= other;
		data[2ull] -= other;
		data[3ull] -= other;
		return *this;
	}

	mat4& mat4::operator*=(const vec4& other) noexcept
	{
		data[0ull] *= other;
		data[1ull] *= other;
		data[2ull] *= other;
		data[3ull] *= other;
		return *this;
	}

	mat4& mat4::operator+=(const float& other) noexcept
	{
		data[0ull] += other;
		data[1ull] += other;
		data[2ull] += other;
		data[3ull] += other;
		return *this;
	}

	mat4& mat4::operator-=(const float& other) noexcept
	{
		data[0ull] -= other;
		data[1ull] -= other;
		data[2ull] -= other;
		data[3ull] -= other;
		return *this;
	}

	mat4& mat4::operator*=(const float& other) noexcept
	{
		data[0ull] *= other;
		data[1ull] *= other;
		data[2ull] *= other;
		data[3ull] *= other;
		return *this;
	}

	mat4 make_mat4(const float* data)
	{
		mat4 mat{};
		mat[0ull] = make_vec4(data);
		mat[1ull] = make_vec4(data + 4ull);
		mat[2ull] = make_vec4(data + 8ull);
		mat[3ull] = make_vec4(data + 12ull);
		return mat;
	}

	mat4 make_mat4(const double* data)
	{
		mat4 mat{};
		mat[0ull] = make_vec4(data);
		mat[1ull] = make_vec4(data + 4ull);
		mat[2ull] = make_vec4(data + 8ull);
		mat[3ull] = make_vec4(data + 12ull);
		return mat;
	}
}