#pragma once

struct FRay
{
	FRay() = default;

	FRay(const math::vec3& origin, const math::vec3& direction)
	{
		m_origin = origin;
		set_direction(direction);
	}

	math::vec3 at(float t) const
	{
		return m_origin + m_direction * t;
	}

	void set_direction(const math::vec3& direction)
	{
		m_direction = direction;
		m_inv_direction = 1.f / m_direction;
	}

	math::vec3 m_origin{};
	math::vec3 m_direction{};
	math::vec3 m_inv_direction{};
};

struct FHitResult
{
	math::vec3 m_color{};
	math::vec3 m_position{};
	math::vec3 m_normal{};
	math::vec3 m_tangent{};
	math::vec3 m_bitangent{};
	math::vec2 m_texcoord{};
	float m_distance{ std::numeric_limits<float>::infinity() };
	bool m_bFrontFace{ false };
	resource_id_t m_material_id{ invalid_index };

	bool is_hit() const 
	{
		return m_distance < std::numeric_limits<float>::infinity();
	}

	inline void set_face_normal(const FRay& ray, const math::vec3& outward_normal)
	{
		m_bFrontFace = math::dot(ray.m_direction, outward_normal) < 0.f;
		m_normal = math::normalize(m_bFrontFace ? outward_normal : outward_normal * -1.f);
	}
};