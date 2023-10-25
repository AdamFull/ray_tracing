#pragma once

#include "aabb.h"
#include "resources/vertex.h"

class CTriangle
{
public:
	CTriangle(entt::registry& registry, entt::entity root, resource_id_t material_id, const FVertex& v0, const FVertex& v1, const FVertex& v2, size_t index);
	~CTriangle() = default;

	void create();

	bool hit(const FRay& r, float t_min, float t_max, FHitResult& hit_result) const;
	float pdf(const glm::vec3& p, const glm::vec3& wi) const;
	const FAxixAlignedBoundingBox& bounds() const;
	const glm::vec3 centroid() const;
	resource_id_t get_material_id() const;

	glm::vec3 sample(const glm::vec3& p, const glm::vec2& sample, float& pdf) const;

private:
	FVertex m_v0, m_v1, m_v2;
	glm::vec3 m_e0, m_e1;
	glm::vec3 m_centroid{};
	glm::mat3 m_normal{ 1.f };
	float m_area{ 0.f };

	FAxixAlignedBoundingBox m_aabb{};
	resource_id_t m_material_id{ invalid_index };
	size_t m_index{ invalid_index };

	entt::entity m_root{ entt::null };
	entt::registry* m_pRegistry{ nullptr };
};