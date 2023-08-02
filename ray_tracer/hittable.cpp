#include "hittable.h"

#include "ecs/components/transform_component.h"

bool ray_triangle_intersect(const glm::vec3& r0, const glm::vec3& rd, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, float& distance, glm::vec3& barycentric)
{
	auto e0 = v1 - v0;
	auto e1 = v2 - v0;
	auto pvec = glm::cross(rd, e1);
	auto det = glm::dot(e0, pvec);

	if (det < std::numeric_limits<float>::epsilon())
		return false;

	float invDet = 1.f / det;

	auto tvec = r0 - v0;
	barycentric.y = glm::dot(tvec, pvec) * invDet;

	if (barycentric.y < 0.f || barycentric.y > 1.f)
		return false;

	auto qvec = glm::cross(tvec, e0);
	barycentric.z = glm::dot(rd, qvec) * invDet;

	if (barycentric.z < 0.f || barycentric.y + barycentric.z > 1.f)
		return false;

	barycentric.x = 1.f - barycentric.y - barycentric.z;

	distance = glm::dot(e1, qvec) * invDet;

	return true;
}

CTriangle::CTriangle(entt::registry& registry, entt::entity root, resource_id_t material, const FVertex& v0, const FVertex& v1, const FVertex& v2) : 
	m_v0(v0), m_v1(v1), m_v2(v2)
{
	m_aabb.m_min = glm::min(m_v0.position, glm::min(m_v1.position, m_v2.position));
	m_aabb.m_max = glm::max(m_v0.position, glm::max(m_v1.position, m_v2.position));

	m_material_id = material;

	m_pRegistry = &registry;
	m_root = root;
}

bool CTriangle::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const
{
	auto& transform = m_pRegistry->get<FTransformComponent>(m_root);
	auto normal = glm::mat3(transform.m_normal);

	auto v0p = transform.m_model * glm::vec4(m_v0.position, 1.f);
	auto v1p = transform.m_model * glm::vec4(m_v1.position, 1.f);
	auto v2p = transform.m_model * glm::vec4(m_v2.position, 1.f);

	auto v0 = glm::vec3(v0p) / v0p.w;
	auto v1 = glm::vec3(v1p) / v1p.w;
	auto v2 = glm::vec3(v2p) / v2p.w;

	glm::vec3 barycentric{};
	float dist{ 0.f };
	if (ray_triangle_intersect(ray.m_origin, ray.m_direction, v0, v1, v2, dist, barycentric))
	{
		if (dist >= t_min && dist <= t_max)
		{
			hit_result.m_distance = dist;
			hit_result.m_position = ray.at(dist);

			// Calculating color
			hit_result.m_color = (barycentric.x * m_v0.color) + (barycentric.y * m_v1.color) + (barycentric.z * m_v2.color);

			// Calculating normal
			auto outward_normal = (barycentric.x * m_v0.normal) + (barycentric.y * m_v1.normal) + (barycentric.z * m_v2.normal);
			outward_normal = glm::normalize(normal * outward_normal);
			hit_result.set_face_normal(ray, outward_normal);

			// Calculating texture coordinates
			hit_result.m_texcoord = barycentric.x * m_v0.texcoord + barycentric.y * m_v1.texcoord + barycentric.z * m_v2.texcoord;

			// Calculate tangent
			glm::vec2 deltaUV1 = m_v1.texcoord - m_v0.texcoord;
			glm::vec2 deltaUV2 = m_v2.texcoord - m_v0.texcoord;
			float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

			glm::vec3 edge1 = v1 - v0;
			glm::vec3 edge2 = v2 - v0;
			glm::vec3 tangent;

			tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
			tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
			tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

			tangent = glm::normalize(tangent);
			hit_result.m_tangent = (barycentric.x * tangent) + (barycentric.y * tangent) + (barycentric.z * tangent);
			hit_result.m_tangent = glm::normalize(normal * hit_result.m_tangent);
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

bool CTriangle::bounds(FAxixAlignedBoundingBox& output_box) const
{
	auto& transform = m_pRegistry->get<FTransformComponent>(m_root);

	auto v0p = transform.m_model * glm::vec4(m_v0.position, 1.f);
	auto v1p = transform.m_model * glm::vec4(m_v1.position, 1.f);
	auto v2p = transform.m_model * glm::vec4(m_v2.position, 1.f);

	auto v0_transformed = glm::vec3(v0p) / v0p.w;
	auto v1_transformed = glm::vec3(v1p) / v1p.w;
	auto v2_transformed = glm::vec3(v2p) / v2p.w;

	output_box.m_min = glm::min(v0_transformed, glm::min(v1_transformed, v2_transformed));
	output_box.m_max = glm::max(v0_transformed, glm::max(v1_transformed, v2_transformed));

	return true;
}