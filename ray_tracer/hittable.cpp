#include "hittable.h"
#include "ecs/components/transform_component.h"

CTriangle::CTriangle(entt::registry& registry, entt::entity root, resource_id_t material_id, const FVertex& v0, const FVertex& v1, const FVertex& v2) :
	m_v0(v0), m_v1(v1), m_v2(v2)
{
	m_material_id = material_id;
	m_pRegistry = &registry;
	m_root = root;
}

void CTriangle::create()
{
	auto& transform = m_pRegistry->get<FTransformComponent>(m_root);

	auto glm_v0p = transform.m_model * glm::vec4(m_v0.position.x, m_v0.position.y, m_v0.position.z, 1.f);
	auto glm_v1p = transform.m_model * glm::vec4(m_v1.position.x, m_v1.position.y, m_v1.position.z, 1.f);
	auto glm_v2p = transform.m_model * glm::vec4(m_v2.position.x, m_v2.position.y, m_v2.position.z, 1.f);

	auto v0p = math::vec4(glm_v0p.x, glm_v0p.y, glm_v0p.z, glm_v0p.w);
	auto v1p = math::vec4(glm_v1p.x, glm_v1p.y, glm_v1p.z, glm_v1p.w);
	auto v2p = math::vec4(glm_v2p.x, glm_v2p.y, glm_v2p.z, glm_v2p.w);

	m_v0.position = math::to_vec3(v0p) / v0p.w;
	m_v1.position = math::to_vec3(v1p) / v1p.w;
	m_v2.position = math::to_vec3(v2p) / v2p.w;

	m_aabb.m_min = math::min(m_v0.position, math::min(m_v1.position, m_v2.position));
	m_aabb.m_max = math::max(m_v0.position, math::max(m_v1.position, m_v2.position));

	m_normal = math::mat3(
		transform.m_normal[0][0], transform.m_normal[0][1], transform.m_normal[0][2],
		transform.m_normal[1][0], transform.m_normal[1][1], transform.m_normal[1][2],
		transform.m_normal[2][0], transform.m_normal[2][1], transform.m_normal[2][2]);
}

bool CTriangle::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const
{
	math::vec3 barycentric{};
	float dist{ 0.f };
	if (math::ray_triangle_intersect(ray.m_origin, ray.m_direction, m_v0.position, m_v1.position, m_v2.position, dist, barycentric))
	{
		if (dist >= t_min && dist <= t_max)
		{
			hit_result.m_distance = dist;
			hit_result.m_position = ray.at(dist);

			// Calculating color
			//hit_result.m_color = math::interpolate(m_v0.color, m_v1.color, m_v2.color, barycentric);
			hit_result.m_color = barycentric.x * m_v0.color + barycentric.y * m_v1.color + barycentric.z * m_v2.color;

			// Calculating normal
			//auto outward_normal = math::interpolate(m_v0.normal, m_v1.normal, m_v2.normal, barycentric);
			auto outward_normal = barycentric.x * m_v0.normal + barycentric.y * m_v1.normal + barycentric.z * m_v2.normal;
			outward_normal = math::normalize(m_normal * outward_normal);
			hit_result.set_face_normal(ray, outward_normal);

			// Calculating texture coordinates
			hit_result.m_texcoord = barycentric.x * m_v0.texcoord + barycentric.y * m_v1.texcoord + barycentric.z * m_v2.texcoord;

			// Calculate tangent
			math::vec2 deltaUV1 = m_v1.texcoord - m_v0.texcoord;
			math::vec2 deltaUV2 = m_v2.texcoord - m_v0.texcoord;
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			math::vec3 edge1 = m_v1.position - m_v0.position;
			math::vec3 edge2 = m_v2.position - m_v0.position;
			math::vec3 tangent;

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			tangent = math::normalize(tangent);
			//hit_result.m_tangent = math::interpolate(tangent, tangent, tangent, barycentric);
			hit_result.m_tangent = barycentric.x * tangent + barycentric.y * tangent + barycentric.z * tangent;
			hit_result.m_tangent = math::normalize(m_normal * hit_result.m_tangent);
			hit_result.m_tangent = math::normalize(hit_result.m_tangent - math::dot(hit_result.m_tangent, hit_result.m_normal) * hit_result.m_normal);

			hit_result.m_bitangent = math::cross(hit_result.m_normal, hit_result.m_tangent);

			if (math::dot(math::cross(hit_result.m_normal, hit_result.m_tangent), hit_result.m_bitangent) < 0.f)
			{
				hit_result.m_tangent = hit_result.m_tangent * -1.f;
				hit_result.m_bitangent = hit_result.m_bitangent * -1.f;
			}

			// Set material
			hit_result.m_material_id = m_material_id;

			return true;
		}
	}

	return false;
}

bool CTriangle::bounds(FAxixAlignedBoundingBox& output_box) const
{
	output_box = m_aabb;
	return true;
}