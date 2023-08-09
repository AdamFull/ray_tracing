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

inline glm::vec3 random_vec3()
{
	return glm::vec3(random<float>(), random<float>(), random<float>());
}

inline glm::vec3 random_vec3(float min, float max)
{
	auto x = random<float>(min, max);
	auto y = random<float>(min, max);
	auto z = random<float>(min, max);
	return glm::vec3(x, y, z);
}

inline glm::vec3 random_unit_vec3()
{
	return glm::normalize(random_vec3(-1.f, 1.f));
}

template<class _Ty>
inline glm::vec3 random_unit_vec3d()
{
	constexpr const _Ty min{ static_cast<_Ty>(-1) }, max{ static_cast<_Ty>(1) };
	while (true)
	{
		auto p = random_unit_vec3<_Ty>();
		p[2] = static_cast<_Ty>(0);
		if (glm::length2(p) >= max)
			continue;
		return p;
	}
}

inline glm::vec3 random_cosine_direction() 
{
	auto r1 = random<float>();
	auto r2 = random<float>();
	auto z = glm::sqrt(1.f - r2);

	auto phi = 2.f * std::numbers::pi_v<float> * r1;
	auto x = glm::cos(phi) * glm::sqrt(r2);
	auto y = glm::sin(phi) * glm::sqrt(r2);

	return glm::vec3(x, y, z);
}

class COrthoNormalBasis
{
public:
	COrthoNormalBasis() = default;

	inline glm::vec3& operator[](size_t index)
	{
		return m_axis[index];
	}

	inline glm::vec3& u() { return m_axis[0ull]; }
	inline glm::vec3& v() { return m_axis[1ull]; }
	inline glm::vec3& w() { return m_axis[2ull]; }

	inline glm::vec3 local(float a, float b, float c)
	{
		return a * u() + b * v() + c * w();
	}

	inline glm::vec3 local(const glm::vec3& a)
	{
		return a.x * u() + a.y * v() + a.z * w();
	}

	void build_from_w(const glm::vec3& n)
	{
		m_axis[2ull] = glm::normalize(n);
		glm::vec3 a = (std::fabs(w().x) > 0.9) ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(1.f, 0.f, 0.f);
		m_axis[1ull] = glm::normalize(glm::cross(w(), a));
		m_axis[0ull] = glm::cross(w(), v());
	}
private:
	glm::vec3 m_axis[3ull];
};