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
	float dist{ hit_result.m_distance };
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_INTERSECTION) && 0
	auto _r0 = _mm_setr_ps(ray.m_origin.x, ray.m_origin.y, ray.m_origin.z, 0.f);
	auto _rd = _mm_setr_ps(ray.m_direction.x, ray.m_direction.y, ray.m_direction.z, 0.f);
	auto _p0 = _mm_setr_ps(m_v0.m_position.x, m_v0.m_position.y, m_v0.m_position.z, 0.f);
	auto _p1 = _mm_setr_ps(m_v1.m_position.x, m_v1.m_position.y, m_v1.m_position.z, 0.f);
	auto _p2 = _mm_setr_ps(m_v2.m_position.x, m_v2.m_position.y, m_v2.m_position.z, 0.f);

	__m128 _bcX, _bcY, _bcZ;
	if (math::ray_triangle_intersect(_r0, _rd, _p0, _p1, _p2, dist, _bcX, _bcY, _bcZ))
	{
		if (dist >= t_min && dist <= t_max)
		{
			hit_result.m_distance = dist;
			hit_result.m_position = ray.at(dist);

			// Interpolate color
			__m128 _c0 = _mm_setr_ps(m_v0.m_color.x, m_v0.m_color.y, m_v0.m_color.z, 0.f);
			__m128 _c1 = _mm_setr_ps(m_v1.m_color.x, m_v1.m_color.y, m_v1.m_color.z, 0.f);
			__m128 _c2 = _mm_setr_ps(m_v2.m_color.x, m_v2.m_color.y, m_v2.m_color.z, 0.f);
			__m128 _rc = _mm_add_ps(_mm_mul_ps(_bcX, _c0), _mm_add_ps(_mm_mul_ps(_bcY, _c1), _mm_mul_ps(_bcZ, _c2)));

			float _color[4ull];
			_mm_storeu_ps(_color, _rc);
			hit_result.m_color = glm::vec3(_color[0ull], _color[1ull], _color[2ull]);

			// Load normal matrix
			__m128 _nmr0 = _mm_setr_ps(m_normal[0].x, m_normal[0].y, m_normal[0].z, 0.f);
			__m128 _nmr1 = _mm_setr_ps(m_normal[1].x, m_normal[1].y, m_normal[1].z, 0.f);
			__m128 _nmr2 = _mm_setr_ps(m_normal[2].x, m_normal[2].y, m_normal[2].z, 0.f);

			// Interpolate normals
			__m128 _n0 = _mm_setr_ps(m_v0.m_normal.x, m_v0.m_normal.y, m_v0.m_normal.z, 0.f);
			__m128 _n1 = _mm_setr_ps(m_v1.m_normal.x, m_v1.m_normal.y, m_v1.m_normal.z, 0.f);
			__m128 _n2 = _mm_setr_ps(m_v2.m_normal.x, m_v2.m_normal.y, m_v2.m_normal.z, 0.f);
			__m128 _rn = _mm_add_ps(_mm_mul_ps(_bcX, _n0), _mm_add_ps(_mm_mul_ps(_bcY, _n1), _mm_mul_ps(_bcZ, _n2)));
			_rn = math::_vec128_normalize(math::_mul_vec_to_mat(_rn, _nmr0, _nmr1, _nmr2));

			__m128 _t0 = _mm_setr_ps(m_v0.m_tangent.x, m_v0.m_tangent.y, m_v0.m_tangent.z, 0.f);
			__m128 _t1 = _mm_setr_ps(m_v1.m_tangent.x, m_v1.m_tangent.y, m_v1.m_tangent.z, 0.f);
			__m128 _t2 = _mm_setr_ps(m_v2.m_tangent.x, m_v2.m_tangent.y, m_v2.m_tangent.z, 0.f);
			__m128 _tg = _mm_add_ps(_mm_mul_ps(_bcX, _t0), _mm_add_ps(_mm_mul_ps(_bcY, _t1), _mm_mul_ps(_bcZ, _t2)));
			_tg = math::_vec128_normalize(math::_mul_vec_to_mat(_tg, _nmr0, _nmr1, _nmr2));

			__m128 _bt = math::_vec128_cross(_rn, _tg);

			float _normal[4ull];
			_mm_storeu_ps(_normal, _rn);
			hit_result.set_face_normal(ray, glm::vec3(_normal[0ull], _normal[1ull], _normal[2ull]));

			float _tangentr[4ull];
			_mm_storeu_ps(_tangentr, _tg);
			hit_result.m_tangent = glm::vec3(_tangentr[0ull], _tangentr[1ull], _tangentr[2ull]);

			float _bitangent[4ull];
			_mm_storeu_ps(_bitangent, _bt);
			hit_result.m_bitangent = glm::vec3(_bitangent[0ull], _bitangent[1ull], _bitangent[2ull]);

			// Set material
			hit_result.m_material_id = m_material_id;

			return true;
		}
	}
#else
	glm::vec3 barycentric{};
	//if (math::ray_triangle_intersect_test(ray.m_origin, ray.m_direction, n0, d0, n1, d1, n2, d2, dist, barycentric))
	if(math::ray_triangle_intersect(ray.m_origin, ray.m_direction, m_v0.m_position, m_v1.m_position, m_v2.m_position, dist, barycentric))
	{
		if (dist >= t_min && dist <= t_max)
		{
			hit_result.m_distance = dist;
			hit_result.m_position = ray.at(dist);

			// Calculating color
			hit_result.m_color = barycentric.x * m_v0.m_color + barycentric.y * m_v1.m_color + barycentric.z * m_v2.m_color;

			// Calculating normal
			auto outward_normal = barycentric.x * m_v0.m_normal + barycentric.y * m_v1.m_normal + barycentric.z * m_v2.m_normal;
			outward_normal = math::normalize(m_normal * outward_normal);
			hit_result.set_face_normal(ray, outward_normal);

			// Calculating texture coordinates
			hit_result.m_texcoord = barycentric.x * m_v0.m_texcoord + barycentric.y * m_v1.m_texcoord + barycentric.z * m_v2.m_texcoord;

			auto tangent = barycentric.x * m_v0.m_tangent + barycentric.y * m_v1.m_tangent + barycentric.z * m_v2.m_tangent;
			tangent = glm::vec4(math::normalize(m_normal * glm::vec3(tangent)), tangent.w);
			hit_result.m_tangent = glm::vec3(tangent);

			hit_result.m_bitangent = math::normalize(glm::cross(hit_result.m_normal, glm::vec3(tangent)) * tangent.w);

			// Set material
			hit_result.m_material_id = m_material_id;

			return true;
		}
	}
#endif

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