#pragma once

#include <cstdlib>

//inline void pcg_hash(uint32_t& seed)
//{
//	uint32_t state = seed * 747796405u + 2891336453u;
//	uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
//	seed = (word >> 22u) ^ word;
//}
//
//template<class _Ty>
//inline _Ty random()
//{
//	static uint32_t seed{ 0x0f0f0f0fu };
//	pcg_hash(seed);
//	return static_cast<_Ty>(seed) / static_cast<_Ty>(std::numeric_limits<uint32_t>::max());
//}
//
//template<class _Ty>
//inline _Ty random(_Ty min, _Ty max)
//{
//	return min + (max - min) * random<_Ty>();
//}
//
//inline glm::vec3 random_vec3()
//{
//	return glm::vec3(random<float>(), random<float>(), random<float>());
//}
//
//inline glm::vec3 random_vec3(float min, float max)
//{
//	auto x = random<float>(min, max);
//	auto y = random<float>(min, max);
//	auto z = random<float>(min, max);
//	return glm::vec3(x, y, z);
//}
//
//inline glm::vec3 random_unit_vec3()
//{
//	return glm::normalize(random_vec3(-1.f, 1.f));
//}
//
//template<class _Ty>
//inline glm::vec3 random_unit_vec3d()
//{
//	constexpr const _Ty min{ static_cast<_Ty>(-1) }, max{ static_cast<_Ty>(1) };
//	while (true)
//	{
//		auto p = random_unit_vec3<_Ty>();
//		p[2] = static_cast<_Ty>(0);
//		if (glm::length2(p) >= max)
//			continue;
//		return p;
//	}
//}
//
//inline glm::vec3 random_cosine_direction() 
//{
//	auto r1 = random<float>();
//	auto r2 = random<float>();
//	auto z = math::fsqrt(1.f - r2);
//
//	auto phi = 2.f * std::numbers::pi_v<float> * r1;
//	auto x = glm::cos(phi) * math::fsqrt(r2);
//	auto y = glm::sin(phi) * math::fsqrt(r2);
//
//	return glm::vec3(x, y, z);
//}