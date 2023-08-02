#pragma once

#include <cstdlib>

inline void pcg_hash(uint32_t& seed)
{
	uint32_t state = seed * 747796405u + 2891336453u;
	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
	seed = (word >> 22u) ^ word;
}

template<class _Ty>
inline _Ty random()
{
	static uint32_t seed{ 0x0f0f0f0fu };
	pcg_hash(seed);
	return static_cast<_Ty>(seed) / static_cast<_Ty>(std::numeric_limits<uint32_t>::max());
}

template<class _Ty>
inline _Ty random(_Ty min, _Ty max)
{
	return min + (max - min) * random<_Ty>();
}

template<class _Ty>
inline glm::vec<3, _Ty, glm::defaultp> random_vec3()
{
	return glm::vec<3, _Ty, glm::defaultp>(random<_Ty>(), random<_Ty>(), random<_Ty>());
}

template<class _Ty>
inline glm::vec<3, _Ty, glm::defaultp> random_vec3(_Ty min, _Ty max)
{
	return glm::vec<3, _Ty, glm::defaultp>(random<_Ty>(min, max), random<_Ty>(min, max), random<_Ty>(min, max));
}

template<class _Ty>
inline glm::vec<3, _Ty, glm::defaultp> random_unit_vec3()
{
	constexpr const _Ty min{ static_cast<_Ty>(-1) }, max{ static_cast<_Ty>(1) };
	while (true)
	{
		auto p = random_vec3<_Ty>(min, max);
		if (glm::dot(p, p) >= max)
			continue;
		return p;
	}
}

template<class _Ty>
inline glm::vec<3, _Ty, glm::defaultp> random_unit_vec3d()
{
	constexpr const _Ty min{ static_cast<_Ty>(-1) }, max{ static_cast<_Ty>(1) };
	while (true)
	{
		auto p = random_unit_vec3<_Ty>();
		p[2] = static_cast<_Ty>(0);
		if (glm::dot(p, p) >= max)
			continue;
		return p;
	}
}