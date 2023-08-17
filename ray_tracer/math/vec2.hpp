#pragma once

#include "vector_base.hpp"

namespace math
{
	template<class _Ty>
	struct vec2
	{
		vec2() : x{ static_cast<_Ty>(0) }, y{ static_cast<_Ty>(0) } {}
		vec2(_Ty v) : x{ v }, y{ v } {}
		vec2(_Ty _x, _Ty _y) : x{ _x }, y{ _y } {}

		vec2& operator+=(const vec2& other) noexcept;
		vec2& operator-=(const vec2& other) noexcept;
		vec2& operator*=(const vec2& other) noexcept;
		vec2& operator/=(const vec2& other);

		vec2& operator+=(const _Ty& other) noexcept;
		vec2& operator-=(const _Ty& other) noexcept;
		vec2& operator*=(const _Ty& other) noexcept;
		vec2& operator/=(const _Ty& other);

		vec2 operator-() noexcept;

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
			struct { _Ty x; _Ty y; };
			struct { _Ty u; _Ty v; };

			_Ty data[2ull];
		};
	};


	template<class _Ty>
	constexpr inline bool operator==(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		return _vector_equal<_Ty, 2ull>(lhs.data, rhs.data);
	}

	template<class _Ty>
	constexpr inline bool operator!=(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		return !operator==(lhs, rhs);
	}

	template<class _Ty>
	constexpr inline bool operator<=(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		return _vector_less_equal<_Ty, 2ull>(lhs.data, rhs.data);
	}

	template<class _Ty>
	constexpr inline bool operator>=(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		return _vector_greather_equal<_Ty, 2ull>(lhs.data, rhs.data);
	}

	// vec to vec
	template<class _Ty>
	constexpr inline vec2<_Ty> operator+(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_add<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	constexpr inline vec2<_Ty> operator-(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_sub<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	constexpr inline vec2<_Ty> operator*(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_mul<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator/(const vec2<_Ty>& lhs, const vec2<_Ty>& rhs)
	{
		vec2<_Ty> res{};
		_vector_div<_Ty, 2ull>(lhs.data, rhs.data, res.data);
		return res;
	}

	// vec to scalar
	template<class _Ty>
	inline vec2<_Ty> operator+(const vec2<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_add<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator-(const vec2<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_sub<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator*(const vec2<_Ty>& lhs, const _Ty& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_mul<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator/(const vec2<_Ty>& lhs, const _Ty& rhs)
	{
		vec2<_Ty> res{};
		_vector_div<_Ty, 2ull>(lhs.data, rhs, res.data);
		return res;
	}

	// scalar to vec
	template<class _Ty>
	inline vec2<_Ty> operator+(const _Ty& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_add<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator-(const _Ty& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_sub<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator*(const _Ty& lhs, const vec2<_Ty>& rhs) noexcept
	{
		vec2<_Ty> res{};
		_vector_mul<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> operator/(const _Ty& lhs, const vec2<_Ty>& rhs)
	{
		vec2<_Ty> res{};
		_vector_div<_Ty, 2ull>(lhs, rhs.data, res.data);
		return res;
	}

	// operators
	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator+=(const vec2<_Ty>& other) noexcept
	{
		_vector_add<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator-=(const vec2<_Ty>& other) noexcept
	{
		_vector_sub<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator*=(const vec2<_Ty>& other) noexcept
	{
		_vector_mul<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator/=(const vec2<_Ty>& other)
	{
		_vector_div<_Ty, 2ull>(data, other.data);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator+=(const _Ty& other) noexcept
	{
		_vector_add<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator-=(const _Ty& other) noexcept
	{
		_vector_sub<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator*=(const _Ty& other) noexcept
	{
		_vector_mul<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty>& vec2<_Ty>::operator/=(const _Ty& other)
	{
		_vector_div<_Ty, 2ull>(data, other);
		return *this;
	}

	template<class _Ty>
	inline vec2<_Ty> vec2<_Ty>::operator-() noexcept
	{
		vec2<_Ty> res{};
		_vector_mul<_Ty, 2ull>(data, -1.f);
		return res;
	}

	template<class _Ty>
	inline vec2<_Ty> make_vec2(const _Ty* vec)
	{
		return vec2<_Ty>(vec[0], vec[1]);
	}
}