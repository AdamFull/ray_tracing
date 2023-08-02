#pragma once

#include "shared.h"

struct FAxixAlignedBoundingBox
{
	FAxixAlignedBoundingBox() = default;
	FAxixAlignedBoundingBox(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max) {}

	bool hit(const FRay& ray, float t_min, float t_max) const
	{
		for (uint32_t idx = 0; idx < 3; ++idx)
		{
			auto invD = 1.f / ray.m_direction[idx];
			auto t0 = (m_min[idx] - ray.m_origin[idx]) * invD;
			auto t1 = (m_max[idx] - ray.m_origin[idx]) * invD;

			if (invD < 0.0f)
				std::swap(t0, t1);

			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;

			if (t_max <= t_min)
				return false;
		}

		return true;
	}

	FAxixAlignedBoundingBox surrounding(const FAxixAlignedBoundingBox& rhs)
	{
		glm::vec3 small{ glm::min(m_min.x, rhs.m_min.x), glm::min(m_min.y, rhs.m_min.y), glm::min(m_min.z, rhs.m_min.z) };
		glm::vec3 big{ glm::max(m_max.x, rhs.m_max.x), glm::max(m_max.y, rhs.m_max.y), glm::max(m_max.z, rhs.m_max.z) };

		return FAxixAlignedBoundingBox(small, big);
	}

	glm::vec3 m_min{};
	glm::vec3 m_max{};
};