#include "hittable.h"
#include "ecs/components/transform_component.h"

glm::vec2 sample_uniform_triangle(const glm::vec2& sample)
{
	float a = glm::sqrt(sample.s);
	return glm::vec2(1.f - a, sample.t * a);
}

void pretransform(const glm::mat4& tm, glm::vec3& v0, glm::vec3& v1, glm::vec3& v2, bool normalize = false)
{
	auto t0 = tm * glm::vec4(v0, 1.f);
	auto t1 = tm * glm::vec4(v1, 1.f);
	auto t2 = tm * glm::vec4(v2, 1.f);

	v0 = normalize ? glm::normalize(glm::vec3(t0) / t0.w) : glm::vec3(t0) / t0.w;
	v1 = normalize ? glm::normalize(glm::vec3(t1) / t1.w) : glm::vec3(t1) / t1.w;
	v2 = normalize ? glm::normalize(glm::vec3(t2) / t2.w) : glm::vec3(t2) / t2.w;
}

void pretransform(const glm::mat4& tm, glm::vec4& v0, glm::vec4& v1, glm::vec4& v2, bool normalize = false)
{
	auto t0 = tm * glm::vec4(v0.x, v0.y, v0.z, 1.f);
	auto t1 = tm * glm::vec4(v1.x, v1.y, v1.z, 1.f);
	auto t2 = tm * glm::vec4(v2.x, v2.y, v2.z, 1.f);

	v0 = glm::vec4(normalize ? glm::normalize(glm::vec3(t0) / t0.w) : glm::vec3(t0) / t0.w, v0.w);
	v1 = glm::vec4(normalize ? glm::normalize(glm::vec3(t1) / t1.w) : glm::vec3(t1) / t1.w, v1.w);
	v2 = glm::vec4(normalize ? glm::normalize(glm::vec3(t2) / t2.w) : glm::vec3(t2) / t2.w, v2.w);
}

CTriangle::CTriangle(entt::registry& registry, entt::entity root, resource_id_t material_id, const FVertex& v0, const FVertex& v1, const FVertex& v2, size_t index) :
	m_v0(v0), m_v1(v1), m_v2(v2)
{
	m_material_id = material_id;
	m_index = index;
	m_pRegistry = &registry;
	m_root = root;
}

void CTriangle::create()
{
	auto& transform = m_pRegistry->get<FTransformComponent>(m_root);

	// Pretransform positions
	auto v0p = transform.m_model * glm::vec4(m_v0.m_position, 1.f);
	auto v1p = transform.m_model * glm::vec4(m_v1.m_position, 1.f);
	auto v2p = transform.m_model * glm::vec4(m_v2.m_position, 1.f);

	m_v0.m_position = glm::vec3(v0p) / v0p.w;
	m_v1.m_position = glm::vec3(v1p) / v1p.w;
	m_v2.m_position = glm::vec3(v2p) / v2p.w;

	m_e0 = m_v1.m_position - m_v0.m_position;
	m_e1 = m_v2.m_position - m_v0.m_position;

	m_area = 0.5f * glm::length(glm::cross(m_e0, m_e1));

	m_aabb.m_min = glm::min(m_v0.m_position, glm::min(m_v1.m_position, m_v2.m_position));
	m_aabb.m_max = glm::max(m_v0.m_position, glm::max(m_v1.m_position, m_v2.m_position));

	m_normal = glm::mat3(transform.m_normal);

	m_centroid = (m_v0.m_position + m_v1.m_position + m_v2.m_position) * 0.3333f;
}

bool CTriangle::hit(const FRay& ray, float t_min, float t_max, FHitResult& hit_result) const
{
	float dist{ hit_result.m_distance };
	glm::vec3 barycentric{};
	if(math::ray_triangle_intersect(ray.m_origin, ray.m_direction, m_e0, m_e1, m_v0.m_position, dist, barycentric))
	{
		if (dist >= t_min && dist <= t_max)
		{
			hit_result.m_distance = dist;
			hit_result.m_position = ray.at(dist);

			// Calculating color
			hit_result.m_color = barycentric.x * m_v0.m_color + barycentric.y * m_v1.m_color + barycentric.z * m_v2.m_color;

			// Calculating normal
			auto outward_normal = barycentric.x * m_v0.m_normal + barycentric.y * m_v1.m_normal + barycentric.z * m_v2.m_normal;
			outward_normal = glm::normalize(m_normal * outward_normal);
			hit_result.set_face_normal(ray, outward_normal);

			// Calculating texture coordinates
			hit_result.m_texcoord = barycentric.x * m_v0.m_texcoord + barycentric.y * m_v1.m_texcoord + barycentric.z * m_v2.m_texcoord;

			auto tangent = barycentric.x * m_v0.m_tangent + barycentric.y * m_v1.m_tangent + barycentric.z * m_v2.m_tangent;
			tangent = glm::vec4(glm::normalize(m_normal * glm::vec3(tangent)), tangent.w);
			hit_result.m_tangent = glm::vec3(tangent);

			hit_result.m_bitangent = glm::normalize(glm::cross(hit_result.m_normal, glm::vec3(tangent)) * tangent.w);

			// Set material
			hit_result.m_material_id = m_material_id;
			hit_result.m_primitive_id = m_index;

			return true;
		}
	}

	return false;
}

float CTriangle::pdf(const glm::vec3& p, const glm::vec3& wi) const
{
	FRay ray{};
	ray.m_origin = p;
	ray.set_direction(wi);

	FHitResult hit_result{};
	bool hit_something = hit(ray, 0.001f, std::numeric_limits<float>::infinity(), hit_result);

	if (!hit_something)
		return 0.f;

	float cosThetaI = glm::dot(-wi, hit_result.m_normal);
	if (cosThetaI <= 0.f)
		return 0.f;

	float square_dist = glm::length2(hit_result.m_position - p);
	return square_dist / (cosThetaI * m_area);
}

const FAxixAlignedBoundingBox& CTriangle::bounds() const
{
	return m_aabb;
}

const glm::vec3 CTriangle::centroid() const
{
	return m_centroid;
}

resource_id_t CTriangle::get_material_id() const
{
	return m_material_id;
}

glm::vec3 CTriangle::sample(const glm::vec3& p, const glm::vec2& sample, float& pdf) const
{
	glm::vec2 uv = sample_uniform_triangle(sample);
	float w = (1.f - uv.x - uv.y);
	glm::vec3 q = uv.x * m_v0.m_position + uv.y * m_v1.m_position + w * m_v2.m_position;
	glm::vec3 dir = glm::normalize(q - p);

	glm::vec3 normal = uv.x * m_v0.m_normal + uv.y * m_v1.m_normal + w * m_v2.m_normal;
	normal = glm::normalize(m_normal * normal);

	float cosThetaI = glm::dot(-dir, normal);

	if (cosThetaI <= 0.f)
		pdf = 0.f;
	else
		pdf = glm::length2(q - p) / cosThetaI;

	return dir;
}