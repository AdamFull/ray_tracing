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

	auto v0p = transform.m_model * glm::vec4(m_v0.m_position, 1.f);
	auto v1p = transform.m_model * glm::vec4(m_v1.m_position, 1.f);
	auto v2p = transform.m_model * glm::vec4(m_v2.m_position, 1.f);

	m_v0.m_position = glm::vec3(v0p) / v0p.w;
	m_v1.m_position = glm::vec3(v1p) / v1p.w;
	m_v2.m_position = glm::vec3(v2p) / v2p.w;

	m_aabb.m_min = math::min(m_v0.m_position, math::min(m_v1.m_position, m_v2.m_position));
	m_aabb.m_max = math::max(m_v0.m_position, math::max(m_v1.m_position, m_v2.m_position));

	m_normal = glm::mat3(transform.m_normal[0][0]);

	m_centroid = (m_v0.m_position + m_v1.m_position + m_v2.m_position) * 0.3333f;
}

bool CTriangle::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const
{
	glm::vec3 barycentric{};
	float dist{ 0.f };
	if (math::ray_triangle_intersect(ray.m_origin, ray.m_direction, m_v0.m_position, m_v1.m_position, m_v2.m_position, dist, barycentric))
	{
		if (dist >= t_min && dist <= t_max)
		{
			hit_result.m_distance = dist;
			hit_result.m_position = ray.at(dist);

			// Calculating color
			//hit_result.m_color = glm::interpolate(m_v0.color, m_v1.color, m_v2.color, barycentric);
			hit_result.m_color = barycentric.x * m_v0.m_color + barycentric.y * m_v1.m_color + barycentric.z * m_v2.m_color;

			// Calculating normal
			//auto outward_normal = glm::interpolate(m_v0.normal, m_v1.normal, m_v2.normal, barycentric);
			auto outward_normal = barycentric.x * m_v0.m_normal + barycentric.y * m_v1.m_normal + barycentric.z * m_v2.m_normal;
			outward_normal = glm::normalize(m_normal * outward_normal);
			hit_result.set_face_normal(ray, outward_normal);

			// Calculating texture coordinates
			hit_result.m_texcoord = barycentric.x * m_v0.m_texcoord + barycentric.y * m_v1.m_texcoord + barycentric.z * m_v2.m_texcoord;

			// Calculate tangent
			glm::vec2 deltaUV1 = m_v1.m_texcoord - m_v0.m_texcoord;
			glm::vec2 deltaUV2 = m_v2.m_texcoord - m_v0.m_texcoord;
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 edge1 = m_v1.m_position - m_v0.m_position;
			glm::vec3 edge2 = m_v2.m_position - m_v0.m_position;
			glm::vec3 tangent;

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			tangent = glm::normalize(tangent);
			//hit_result.m_tangent = glm::interpolate(tangent, tangent, tangent, barycentric);
			hit_result.m_tangent = barycentric.x * tangent + barycentric.y * tangent + barycentric.z * tangent;
			hit_result.m_tangent = glm::normalize(m_normal * hit_result.m_tangent);
			hit_result.m_tangent = glm::normalize(hit_result.m_tangent - glm::dot(hit_result.m_tangent, hit_result.m_normal) * hit_result.m_normal);

			hit_result.m_bitangent = glm::cross(hit_result.m_normal, hit_result.m_tangent);

			if (glm::dot(glm::cross(hit_result.m_normal, hit_result.m_tangent), hit_result.m_bitangent) < 0.f)
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

const FAxixAlignedBoundingBox& CTriangle::bounds() const
{
	return m_aabb;
}

const glm::vec3 CTriangle::centroid() const
{
	return m_centroid;
}