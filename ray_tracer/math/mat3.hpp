#pragma once

#include "vec3_math.hpp"

namespace math
{
	struct mat3
	{
		mat3()
		{
			data[0ull].x = 1.f;
			data[1ull].y = 1.f;
			data[2ull].z = 1.f;
		}

		mat3(float v)
		{
			data[0ull].x = v;
			data[1ull].y = v;
			data[2ull].z = v;
		}

		mat3(
			float x0, float y0, float z0,
			float x1, float y1, float z1,
			float x2, float y2, float z2)
		{
			data[0ull].x = x0;
			data[0ull].y = y0;
			data[0ull].z = z0;

			data[1ull].x = x1;
			data[1ull].y = y1;
			data[1ull].z = z1;

			data[2ull].x = x2;
			data[2ull].y = y2;
			data[2ull].z = z2;
		}

		mat3(const vec3& v0, const vec3& v1, const vec3& v2)
		{
			data[0ull] = v0;
			data[1ull] = v1;
			data[2ull] = v2;
		}

		mat3& operator+=(const mat3& other) noexcept;
		mat3& operator-=(const mat3& other) noexcept;
		mat3& operator*=(const mat3& other) noexcept;

		mat3& operator+=(const vec3& other) noexcept;
		mat3& operator-=(const vec3& other) noexcept;
		mat3& operator*=(const vec3& other) noexcept;

		mat3& operator+=(const float& other) noexcept;
		mat3& operator-=(const float& other) noexcept;
		mat3& operator*=(const float& other) noexcept;

		const vec3& operator[](size_t index) const
		{
			return data[index];
		}

		vec3& operator[](size_t index)
		{
			return data[index];
		}

		vec3 data[3ull];
	};

	constexpr inline bool operator==(const mat3& lhs, const mat3& rhs) noexcept
	{
		return lhs[0ull] == rhs[0ull] && lhs[1ull] == rhs[1ull] && lhs[2ull] == rhs[2ull];
	}

	constexpr inline bool operator!=(const mat3& lhs, const mat3& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	constexpr inline bool operator<=(const mat3& lhs, const mat3& rhs) noexcept
	{
		return lhs[0ull] >= rhs[0ull] && lhs[1ull] >= rhs[1ull] && lhs[2ull] >= rhs[2ull];
	}

	constexpr inline bool operator>=(const mat3& lhs, const mat3& rhs) noexcept
	{
		return lhs[0ull] <= rhs[0ull] && lhs[1ull] <= rhs[1ull] && lhs[2ull] <= rhs[2ull];
	}

	// mat to mat
	inline mat3 operator+(const mat3& lhs, const mat3& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] + rhs[0ull];
		res[1ull] = lhs[1ull] + rhs[1ull];
		res[2ull] = lhs[2ull] + rhs[2ull];
		return res;
	}

	inline mat3 operator-(const mat3& lhs, const mat3& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] - rhs[0ull];
		res[1ull] = lhs[1ull] - rhs[1ull];
		res[2ull] = lhs[2ull] - rhs[2ull];
		return res;
	}

	inline mat3 operator*(const mat3& lhs, const mat3& rhs) noexcept
	{
		mat3 res{ 0.f };
		for (size_t r = 0ull; r < 3ull; ++r)
		{
			for (size_t c = 0ull; c < 3ull; ++c)
			{
				res[r][c] = 0.f;
				for (size_t k = 0ull; k < 3ull; ++k)
					res[r][c] += lhs[r][k] * rhs[k][c];
			}
		}
		return res;
	}

	// mat to vector
	inline mat3 operator+(const mat3& lhs, const vec3& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] + rhs;
		res[1ull] = lhs[1ull] + rhs;
		res[2ull] = lhs[2ull] + rhs;
		return res;
	}

	inline mat3 operator-(const mat3& lhs, const vec3& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] - rhs;
		res[1ull] = lhs[1ull] - rhs;
		res[2ull] = lhs[2ull] - rhs;
		return res;
	}

	inline vec3 operator*(const mat3& lhs, const vec3& rhs) noexcept
	{
		vec3 res{};
		res += lhs[0ull] * rhs;
		res += lhs[1ull] * rhs;
		res += lhs[2ull] * rhs;
		return res;
	}

	inline vec3 operator*(const vec3& lhs, const mat3& rhs) noexcept
	{
		vec3 res{};
		res += lhs * rhs[0ull];
		res += lhs * rhs[1ull];
		res += lhs * rhs[2ull];
		return res;
	}

	// mat to scalar
	inline mat3 operator+(const mat3& lhs, const float& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] + rhs;
		res[1ull] = lhs[1ull] + rhs;
		res[2ull] = lhs[2ull] + rhs;
		return res;
	}

	inline mat3 operator-(const mat3& lhs, const float& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] - rhs;
		res[1ull] = lhs[1ull] - rhs;
		res[2ull] = lhs[2ull] - rhs;
		return res;
	}

	inline mat3 operator*(const mat3& lhs, const float& rhs) noexcept
	{
		mat3 res{};
		res[0ull] = lhs[0ull] * rhs;
		res[1ull] = lhs[1ull] * rhs;
		res[2ull] = lhs[2ull] * rhs;
		return res;
	}

	// operators
	mat3& mat3::operator+=(const mat3& other) noexcept
	{
		data[0ull] += other[0ull];
		data[1ull] += other[1ull];
		data[2ull] += other[2ull];
		return *this;
	}

	mat3& mat3::operator-=(const mat3& other) noexcept
	{
		data[0ull] -= other[0ull];
		data[1ull] -= other[1ull];
		data[2ull] -= other[2ull];
		return *this;
	}

	mat3& mat3::operator*=(const mat3& other) noexcept
	{
		mat3 tmp{ *this };
		for (size_t r = 0ull; r < 3ull; ++r)
		{
			for (size_t c = 0ull; c < 3ull; ++c)
			{
				data[r][c] = 0.f;
				for (size_t k = 0ull; k < 3ull; ++k)
					data[r][c] += tmp[r][k] * other[k][c];
			}
		}
		return *this;
	}

	mat3& mat3::operator+=(const vec3& other) noexcept
	{
		data[0ull] += other;
		data[1ull] += other;
		data[2ull] += other;
		return *this;
	}

	mat3& mat3::operator-=(const vec3& other) noexcept
	{
		data[0ull] -= other;
		data[1ull] -= other;
		data[2ull] -= other;
		return *this;
	}

	mat3& mat3::operator*=(const vec3& other) noexcept
	{
		data[0ull] *= other;
		data[1ull] *= other;
		data[2ull] *= other;
		return *this;
	}

	mat3& mat3::operator+=(const float& other) noexcept
	{
		data[0ull] += other;
		data[1ull] += other;
		data[2ull] += other;
		return *this;
	}

	mat3& mat3::operator-=(const float& other) noexcept
	{
		data[0ull] -= other;
		data[1ull] -= other;
		data[2ull] -= other;
		return *this;
	}

	mat3& mat3::operator*=(const float& other) noexcept
	{
		data[0ull] *= other;
		data[1ull] *= other;
		data[2ull] *= other;
		return *this;
	}

	mat3 make_mat3(const float* data)
	{
		mat3 mat{};
		mat[0ull] = make_vec3(data);
		mat[1ull] = make_vec3(data + 4ull);
		mat[2ull] = make_vec3(data + 8ull);
		return mat;
	}

	mat3 make_mat3(const double* data)
	{
		mat3 mat{};
		mat[0ull] = make_vec3(data);
		mat[1ull] = make_vec3(data + 4ull);
		mat[2ull] = make_vec3(data + 8ull);
		return mat;
	}
}