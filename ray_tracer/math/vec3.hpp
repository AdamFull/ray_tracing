#pragma once

#include "math_template.hpp"

namespace math
{
	template<class _Ty>
	struct vec3
	{
		vec3() : x{ static_cast<_Ty>(0) }, y{ static_cast<_Ty>(0) }, z{ static_cast<_Ty>(0) } {}
		vec3(_Ty v) : x{ v }, y{ v }, z{ v } {}
		vec3(_Ty _x, _Ty _y, _Ty _z)
		{
			x = _x;
			y = _y;
			z = _z;
		}

		vec3& operator+=(const vec3& other) noexcept;
		vec3& operator-=(const vec3& other) noexcept;
		vec3& operator*=(const vec3& other) noexcept;
		vec3& operator/=(const vec3& other);

		vec3& operator+=(const _Ty& other) noexcept;
		vec3& operator-=(const _Ty& other) noexcept;
		vec3& operator*=(const _Ty& other) noexcept;
		vec3& operator/=(const _Ty& other);

		vec3 operator-() noexcept;

		const _Ty& operator[](size_t index) const
		{
			return data[index];
		}

		_Ty& operator[](size_t index)
		{
			return data[index];
		}

		union
		{
			struct { _Ty x; _Ty y; _Ty z; };
			struct { _Ty r; _Ty g; _Ty b; };
			_Ty data[3ull];
		};
	};

	
	template<class _Ty>
	constexpr inline bool operator==(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		return _vector_equal<_Ty, 3ull>(lhs.data, rhs.data);
	}

	template<class _Ty>
	constexpr inline bool operator!=(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	template<class _Ty>
	constexpr inline bool operator<=(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		return _vector_less_equal<_Ty, 3ull>(lhs.data, rhs.data);
	}

	template<class _Ty>
	constexpr inline bool operator>=(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		return _vector_greather_equal<_Ty, 3ull>(lhs.data, rhs.data);
	}

	// vec to vec
	template<class _Ty>
	constexpr inline vec3<_Ty> operator+(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_add<_Ty, 3ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	constexpr inline vec3<_Ty> operator-(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_sub<_Ty, 3ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	constexpr inline vec3<_Ty> operator*(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_mul<_Ty, 3ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator/(const vec3<_Ty>& lhs, const vec3<_Ty>& rhs)
	{
		vec3<_Ty> res{};
		_vector_div<_Ty, 3ull>(lhs.data, rhs.data, res.data);
		return res;
}

	// vec to scalar
	template<class _Ty>
	inline vec3<_Ty> operator+(const vec3<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_add<_Ty, 3ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator-(const vec3<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_sub<_Ty, 3ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator*(const vec3<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_mul<_Ty, 3ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator/(const vec3<_Ty>& lhs, const _Ty& rhs)
	{
		vec3<_Ty> res{};
		_vector_div<_Ty, 3ull>(lhs.data, rhs, res.data);
		return res;
	}

	// scalar to vec
	template<class _Ty>
	inline vec3<_Ty> operator+(const _Ty& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_add<_Ty, 3ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator-(const _Ty& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_sub<_Ty, 3ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator*(const _Ty& lhs, const vec3<_Ty>& rhs) noexcept
	{
		vec3<_Ty> res{};
		_vector_mul<_Ty, 3ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> operator/(const _Ty& lhs, const vec3<_Ty>& rhs)
	{
		vec3<_Ty> res{};
		_vector_div<_Ty, 3ull>(lhs, rhs.data, res.data);
		return res;
	}

	// operators
	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator+=(const vec3<_Ty>& other) noexcept
	{
		_vector_add<_Ty, 3ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator-=(const vec3<_Ty>& other) noexcept
	{
		_vector_sub<_Ty, 3ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator*=(const vec3<_Ty>& other) noexcept
	{
		_vector_mul<_Ty, 3ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator/=(const vec3<_Ty>& other)
	{
		_vector_div<_Ty, 3ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator+=(const _Ty& other) noexcept
	{
		_vector_add<_Ty, 3ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator-=(const _Ty& other) noexcept
	{
		_vector_sub<_Ty, 3ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator*=(const _Ty& other) noexcept
	{
		_vector_mul<_Ty, 3ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty>& vec3<_Ty>::operator/=(const _Ty& other)
	{
		_vector_div<_Ty, 3ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec3<_Ty> vec3<_Ty>::operator-() noexcept
	{
		vec3<_Ty> res{};
		_vector_mul<_Ty, 3ull>(data, -1.f);
		return res;
	}

	template<class _Ty>
	inline vec3<_Ty> make_vec3(const _Ty* vec)
	{
		return vec3<_Ty>(vec[0], vec[1], vec[2]);
	}
}