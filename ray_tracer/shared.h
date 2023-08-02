#pragma once

struct FRay
{
	glm::vec3 at(float t) const
	{
		return m_origin + m_direction * t;
	}

	glm::vec3 m_origin{};
	glm::vec3 m_direction{};
};

struct FHitResult
{
	glm::vec3 m_color{};
	glm::vec3 m_position{};
	glm::vec3 m_normal{};
	glm::vec3 m_tangent{};
	glm::vec3 m_bitangent{};
	glm::vec2 m_texcoord{};
	float m_distance{ std::numeric_limits<float>::infinity() };
	bool m_bFrontFace{ false };
	resource_id_t m_material_id{ invalid_index };

	bool is_hit() const 
	{
		return m_distance < std::numeric_limits<float>::infinity();
	}

	inline void set_face_normal(const FRay& ray, const glm::vec3& outward_normal)
	{
		m_bFrontFace = glm::dot(ray.m_direction, outward_normal) < 0.f;
		m_normal = glm::normalize(m_bFrontFace ? outward_normal : -outward_normal);
	}
};