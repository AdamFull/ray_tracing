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
	auto distance = get_distance(hit_result);

	float attenuation = glm::clamp(1.f - glm::pow(distance, 2.f) / glm::pow(m_light->m_radius, 2.f), 0.f, 1.f);
	return m_light->m_color * m_light->m_intencity * attenuation * attenuation;
}


void CSpotLightSource::create(entt::entity root, const entt::registry& registry)
{
	CLightSource::create(root, registry);
	m_light = m_pRegistry->try_get<FSpotLightComponent>(m_root);
}

glm::vec3 CSpotLightSource::get_direction(const FHitResult& hit_result) const
{
	return glm::normalize(m_light->m_toTarget ? m_transform->m_position_g - m_light->m_target : m_transform->m_rotation_g * glm::vec3(0.f, 0.f, -1.f));
}

float CSpotLightSource::get_distance(const FHitResult& hit_result) const
{
	return std::numeric_limits<float>::infinity();
}

float CSpotLightSource::get_pdf(const FHitResult& hit_result) const
{
	auto light_direction = get_direction(hit_result);
	//auto cos_theta_i = std::cos(light_direction);

	return 1.f;
}

glm::vec3 CSpotLightSource::get_color(const FHitResult& hit_result) const
{
	auto light_angle_scale = 1.f / glm::max(0.001f, glm::cos(m_light->m_innerAngle) - glm::cos(m_light->m_outerAngle));
	auto light_angle_offset = -glm::cos(m_light->m_outerAngle) * light_angle_scale;

	//float cd = glm::dot(m_light->m_toTarget > 0.f ? light_direction : -light_direction, light_direction);
	//float attenuation = glm::clamp(cd * light_angle_scale + light_angle_offset, 0.f, 1.f);
	// TODO: TODO
	return glm::vec3(0.f);
}