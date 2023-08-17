#pragma once

#include "math_template.hpp"

namespace math
{
	template<class _Ty>
	struct vec4
	{
		vec4() : x{ static_cast<_Ty>(0) }, y{ static_cast<_Ty>(0) }, z{ static_cast<_Ty>(0) }, w{ static_cast<_Ty>(0) } {}
		vec4(_Ty v) : x{ v }, y{ v }, z{ v }, w{ v } {}
		vec4(_Ty _x, _Ty _y, _Ty _z, _Ty _w) : x{ _x }, y{ _y }, z{ _z }, w{ _w } {}

		vec4& operator+=(const vec4& other) noexcept;
		vec4& operator-=(const vec4& other) noexcept;
		vec4& operator*=(const vec4& other) noexcept;
		vec4& operator/=(const vec4& other);

		vec4& operator+=(const _Ty& other) noexcept;
		vec4& operator-=(const _Ty& other) noexcept;
		vec4& operator*=(const _Ty& other) noexcept;
		vec4& operator/=(const _Ty& other);

		vec4 operator-() noexcept;

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
			struct { _Ty x; _Ty y; _Ty z; _Ty w; };
			struct { _Ty r; _Ty g; _Ty b; _Ty a; };
			_Ty data[4ull];
		};
	};


	template<class _Ty>
	constexpr inline bool operator==(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		return _vector_equal<_Ty, 2ull>(lhs.data, rhs.data);
	}

	template<class _Ty>
	constexpr inline bool operator!=(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	template<class _Ty>
	constexpr inline bool operator<=(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		return _vector_less_equal<_Ty, 2ull>(lhs.data, rhs.data);
	}

	template<class _Ty>
	constexpr inline bool operator>=(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		return _vector_greather_equal<_Ty, 2ull>(lhs.data, rhs.data);
	}

	// vec to vec
	template<class _Ty>
	constexpr inline vec4<_Ty> operator+(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_add<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	constexpr inline vec4<_Ty> operator-(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_sub<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	constexpr inline vec4<_Ty> operator*(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_mul<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator/(const vec4<_Ty>& lhs, const vec4<_Ty>& rhs)
	{
		vec4<_Ty> res{};
		_vector_div<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	// vec to scalar
	template<class _Ty>
	inline vec4<_Ty> operator+(const vec4<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_add<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator-(const vec4<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_sub<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator*(const vec4<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_mul<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator/(const vec4<_Ty>& lhs, const _Ty& rhs)
	{
		vec4<_Ty> res{};
		_vector_div<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	// scalar to vec
	template<class _Ty>
	inline vec4<_Ty> operator+(const _Ty& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_add<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator-(const _Ty& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_sub<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator*(const _Ty& lhs, const vec4<_Ty>& rhs) noexcept
	{
		vec4<_Ty> res{};
		_vector_mul<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> operator/(const _Ty& lhs, const vec4<_Ty>& rhs)
	{
		vec4<_Ty> res{};
		_vector_div<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	// operators
	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator+=(const vec4<_Ty>& other) noexcept
	{
		_vector_add<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator-=(const vec4<_Ty>& other) noexcept
	{
		_vector_sub<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator*=(const vec4<_Ty>& other) noexcept
	{
		_vector_mul<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator/=(const vec4<_Ty>& other)
	{
		_vector_div<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator+=(const _Ty& other) noexcept
	{
		_vector_add<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator-=(const _Ty& other) noexcept
	{
		_vector_sub<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator*=(const _Ty& other) noexcept
	{
		_vector_mul<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty>& vec4<_Ty>::operator/=(const _Ty& other)
	{
		_vector_div<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec4<_Ty> vec4<_Ty>::operator-() noexcept
	{
		vec4<_Ty> res{};
		_vector_mul<_Ty, 2ull>(data, -1.f);
		return res;
	}

	template<class _Ty>
	inline vec4<_Ty> make_vec4(const _Ty* vec)
	{
		return vec4<_Ty>(vec[0], vec[1], vec[2], vec[3]);
	}
}