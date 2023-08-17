#pragma once

#include "math_template.hpp"

namespace math
{
	template<class _Ty, size_t _Size>
	constexpr inline bool _vector_equal(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
		{
			if (!equal(lhs[idx], rhs[idx]))
				return false;
		}

		return true;
	}

	template<class _Ty, size_t _Size>
	constexpr inline bool _vector_greather_equal(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
		{
			if (!greather_equal(lhs[idx], rhs[idx]))
				return false;
		}

		return true;
	}

	template<class _Ty, size_t _Size>
	constexpr inline bool _vector_less_equal(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
		{
			if (!less_equal(lhs[idx], rhs[idx]))
				return false;
		}

		return true;
	}

	// Vector add operations
	template<class _Ty, size_t _Size>
	constexpr inline void _vector_add(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size], _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] + rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_add(const _Ty(&lhs)[_Size], const _Ty& rhs, _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] + rhs;
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_add(const _Ty& lhs, const _Ty(&rhs)[_Size], _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs + rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_add(_Ty(&res)[_Size], const _Ty(&lhs)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] += lhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_add(_Ty(&res)[_Size], const _Ty& lhs) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] += lhs;
	}


	// Vector sub operations
	template<class _Ty, size_t _Size>
	constexpr inline void _vector_sub(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size], _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] - rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_sub(const _Ty(&lhs)[_Size], const _Ty& rhs, _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] - rhs;
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_sub(const _Ty& lhs, const _Ty(&rhs)[_Size], _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs - rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_sub(_Ty(&res)[_Size], const _Ty(&lhs)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] -= lhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_sub(_Ty(&res)[_Size], const _Ty& lhs) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] -= lhs;
	}


	// Vector mul operations
	template<class _Ty, size_t _Size>
	constexpr inline void _vector_mul(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size], _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] * rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_mul(const _Ty(&lhs)[_Size], const _Ty& rhs, _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] * rhs;
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_mul(const _Ty& lhs, const _Ty(&rhs)[_Size], _Ty(&res)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs * rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_mul(_Ty(&res)[_Size], const _Ty(&lhs)[_Size]) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] *= lhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_mul(_Ty(&res)[_Size], const _Ty& lhs) noexcept
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] *= lhs;
	}


	// Vector div operations
	template<class _Ty, size_t _Size>
	constexpr inline void _vector_div(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size], _Ty(&res)[_Size])
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] / rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_div(const _Ty(&lhs)[_Size], const _Ty& rhs, _Ty(&res)[_Size])
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs[idx] / rhs;
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_div(const _Ty& lhs, const _Ty(&rhs)[_Size], _Ty(&res)[_Size])
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] = lhs / rhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_div(_Ty(&res)[_Size], const _Ty(&lhs)[_Size])
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] /= lhs[idx];
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_div(_Ty(&res)[_Size], const _Ty& lhs)
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res[idx] /= lhs;
	}

	template<class _Ty, size_t _Size>
	constexpr inline void _vector_dot_product(const _Ty(&lhs)[_Size], const _Ty(&rhs)[_Size], _Ty& res)
	{
		for (size_t idx = 0ull; idx < _Size; ++idx)
			res += lhs[idx] * rhs[idx];
	}
}