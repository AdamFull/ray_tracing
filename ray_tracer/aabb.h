#pragma once

#include "shared.h"

struct FAxixAlignedBoundingBox
{
	FAxixAlignedBoundingBox()
	{
		m_min = glm::vec3(std::numeric_limits<float>::max());
		m_max = glm::vec3(-std::numeric_limits<float>::max());
	}

	FAxixAlignedBoundingBox(const glm::vec3& min, const glm::vec3& max) : m_min(min), m_max(max) {}

	glm::vec3 extent() const
	{
		return m_max - m_min;
	}

	float hit(const FRay& ray, const FHitResult& hit_result) const
	{
		return math::ray_aabb_intersect(ray.m_origin, ray.m_inv_direction, m_min, m_max, hit_result.m_distance);
	}

	void grow(const FAxixAlignedBoundingBox& rhs)
	{
		m_min = math::min(m_min, rhs.m_min);
		m_max = math::max(m_max, rhs.m_max);
	}

	float area()
	{
		auto ext = extent();
		return ext.x * ext.y + ext.y * ext.z + ext.z * ext.x;
	}

	glm::vec3 m_min;
	glm::vec3 m_max;
};