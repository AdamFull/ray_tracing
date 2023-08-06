#pragma once

#include "shared.h"

struct FAxixAlignedBoundingBox
{
	FAxixAlignedBoundingBox()
	{
		m_min = math::vec3(std::numeric_limits<float>::max());
		m_max = math::vec3(std::numeric_limits<float>::min());
	}

	FAxixAlignedBoundingBox(const float min, const float max) : m_min(min), m_max(max) {}
	FAxixAlignedBoundingBox(const math::vec3& min, const math::vec3& max) : m_min(min), m_max(max) {}

	math::vec3 extent() const
	{
		return m_max - m_min;
	}

	bool hit(const FRay& ray, float t_min, float t_max) const
	{
		return math::ray_aabb_intersect(ray.m_origin, ray.m_inv_direction, m_min, m_max);
	}

	FAxixAlignedBoundingBox surrounding(const FAxixAlignedBoundingBox& rhs)
	{
		auto small = math::min(m_min, rhs.m_min);
		auto big = math::max(m_max, rhs.m_max);

		return FAxixAlignedBoundingBox(small, big);
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

	math::vec3 m_min;
	math::vec3 m_max;
};