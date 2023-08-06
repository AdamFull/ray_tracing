#pragma once

#include "aabb.h"
#include "resources/vertex.h"

class CHittable
{
public:
	virtual ~CHittable() = default;

	virtual bool hit(const FRay& r, float t_min, float t_max, FHitResult& hit_result) const = 0;
	virtual bool bounds(FAxixAlignedBoundingBox& output_box) const = 0;
protected:
	FAxixAlignedBoundingBox m_aabb{};
};

class CTriangle : public CHittable
{
public:
	CTriangle(entt::registry& registry, entt::entity root, resource_id_t material_id, const FVertex& v0, const FVertex& v1, const FVertex& v2);
	~CTriangle() override = default;

	void create();

	bool hit(const FRay& r, float t_min, float t_max, FHitResult& hit_result) const override;
	bool bounds(FAxixAlignedBoundingBox& output_box) const override;
private:
	FVertex m_v0, m_v1, m_v2;
	math::mat3 m_normal{ 1.f };

	resource_id_t m_material_id{ invalid_index };
	entt::entity m_root{ entt::null };
	entt::registry* m_pRegistry{ nullptr };
};