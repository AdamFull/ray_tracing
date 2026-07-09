#include "light_source.h"

#include "ecs/components/directional_light_component.h"
#include "ecs/components/point_light_component.h"
#include "ecs/components/spot_light_component.h"
#include "ecs/components/transform_component.h"

void CLightSource::create(entt::entity root, const entt::registry& registry)
{
	m_root = root;
	m_pRegistry = &registry;
	m_transform = m_pRegistry->try_get<FTransformComponent>(m_root);
}

void CDirectionalLightSource::create(entt::entity root, const entt::registry& registry)
{
	CLightSource::create(root, registry);
	m_light = m_pRegistry->try_get<FDirectionalLightComponent>(m_root);
}

glm::vec3 CDirectionalLightSource::get_direction(const FHitResult& hit_result) const
{
	return glm::normalize(-(m_transform->m_rotation_g * glm::vec3(0.f, 0.f, -1.f)));
}

float CDirectionalLightSource::get_distance(const FHitResult& hit_result) const
{
	return std::numeric_limits<float>::infinity();
}

float CDirectionalLightSource::get_pdf(const FHitResult& hit_result) const
{
	return 1.f;
}

glm::vec3 CDirectionalLightSource::get_color(const FHitResult& hit_result) const
{
	return m_light->m_color * m_light->m_intencity;
}


void CPointLightSource::create(entt::entity root, const entt::registry& registry)
{
	CLightSource::create(root, registry);
	m_light = m_pRegistry->try_get<FPointLightComponent>(m_root);
}

glm::vec3 CPointLightSource::get_direction(const FHitResult& hit_result) const
{
	return glm::normalize(m_transform->m_position_g - hit_result.m_position);
}

float CPointLightSource::get_distance(const FHitResult& hit_result) const
{
	return glm::distance(hit_result.m_position, m_transform->m_position_g);
}

float CPointLightSource::get_pdf(const FHitResult& hit_result) const
{
	return 1.f;
}

glm::vec3 CPointLightSource::get_color(const FHitResult& hit_result) const
{
	glm::vec3 radiance = m_light->m_color * m_light->m_intencity;

	auto distance = get_distance(hit_result);

	// Physical inverse-square falloff (clamped near zero to avoid the singularity at d = 0).
	float attenuation = 1.f / glm::max(distance * distance, 1e-4f);

	// Optional smooth windowing to zero at the light's range. A non-positive range means
	// "infinite" (glTF KHR_lights_punctual leaves range undefined -> 0), so no window.
	if (m_light->m_radius > 0.f)
	{
		float factor = distance / m_light->m_radius;
		float window = glm::clamp(1.f - factor * factor * factor * factor, 0.f, 1.f);
		attenuation *= window * window;
	}

	return radiance * attenuation;
}


void CSpotLightSource::create(entt::entity root, const entt::registry& registry)
{
	CLightSource::create(root, registry);
	m_light = m_pRegistry->try_get<FSpotLightComponent>(m_root);
}

glm::vec3 CSpotLightSource::get_direction(const FHitResult& hit_result) const
{
	// Direction from the shaded point toward the light (used as the shadow-ray / wi direction).
	return glm::normalize(m_transform->m_position_g - hit_result.m_position);
}

float CSpotLightSource::get_distance(const FHitResult& hit_result) const
{
	return glm::distance(hit_result.m_position, m_transform->m_position_g);
}

float CSpotLightSource::get_pdf(const FHitResult& hit_result) const
{
	return 1.f;
}

glm::vec3 CSpotLightSource::get_color(const FHitResult& hit_result) const
{
	glm::vec3 radiance = m_light->m_color * m_light->m_intencity;

	// The direction the cone is pointing (world space).
	glm::vec3 spot_dir = m_light->m_toTarget
		? glm::normalize(m_light->m_target - m_transform->m_position_g)
		: glm::normalize(m_transform->m_rotation_g * glm::vec3(0.f, 0.f, -1.f));

	// Direction from the light toward the shaded point.
	glm::vec3 to_surface = glm::normalize(hit_result.m_position - m_transform->m_position_g);

	// Angular (cone) attenuation, smoothly ramping from the inner to the outer cone
	// (glTF KHR_lights_punctual formulation).
	float cos_angle = glm::dot(spot_dir, to_surface);
	float light_angle_scale = 1.f / glm::max(0.001f, glm::cos(m_light->m_innerAngle) - glm::cos(m_light->m_outerAngle));
	float light_angle_offset = -glm::cos(m_light->m_outerAngle) * light_angle_scale;

	float angular = glm::clamp(cos_angle * light_angle_scale + light_angle_offset, 0.f, 1.f);
	angular *= angular;

	if (angular <= 0.f)
		return glm::vec3(0.f);

	// Physical inverse-square distance falloff.
	float distance = get_distance(hit_result);
	float attenuation = 1.f / glm::max(distance * distance, 1e-4f);

	return radiance * angular * attenuation;
}