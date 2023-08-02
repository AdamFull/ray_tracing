#include "render_system.h"

#include <algorithm>
#include <execution>

#include "engine.h"
#include "util.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/camera_component.h"
#include "ecs/components/mesh_component.h"

glm::vec3 ray_color(const FRay& ray)
{
	auto direction = glm::normalize(ray.m_direction);
	auto t = 0.5f * (direction.y + 1.0f);
	return glm::vec3(glm::vec3(1.f, 1.f, 1.f) * (1.f - t) + glm::vec3(0.5f, 0.7f, 1.f) * t);
	//return glm::vec3(0.05f);
}

void CRenderSystem::create(CRayEngine* engine)
{
	auto& scene = engine->get_scene();
	scene->build_acceleration();
}

void CRenderSystem::update(CRayEngine* engine)
{
	auto& renderer = engine->get_renderer();
	auto& pixel_iter = renderer->get_pixel_iterator();

	auto& scene = engine->get_scene();
	auto& registry = scene->get_registry();

	FCameraComponent* current_camera{ nullptr };
	FTransformComponent* camera_transform{ nullptr };
	auto view = registry.view<FTransformComponent, FCameraComponent>();
	for (auto [entity, transform, camera] : view.each())
	{
		current_camera = &camera;
		camera_transform = &transform;
	}

	if (!current_camera || !camera_transform)
		return;

	std::for_each(std::execution::par, pixel_iter.begin(), pixel_iter.end(),
		[this, engine, current_camera, camera_transform](uint32_t index)
		{ trace_ray(engine, current_camera, camera_transform, index); });

	auto& framebuffer = renderer->get_framebuffer();
	framebuffer->present();
}

void CRenderSystem::trace_ray(CRayEngine* engine, FCameraComponent* camera, FTransformComponent* camera_transform, uint32_t ray_index)
{
	auto& renderer = engine->get_renderer();
	auto& framebuffer = renderer->get_framebuffer();

	auto& viewport_extent = camera->m_viewportExtent;

	auto x = ray_index % viewport_extent.x;
	auto y = ray_index / viewport_extent.x;

	auto& ray_origin = camera_transform->m_position;
	auto& ray_direction = camera->m_vRayDirections[ray_index];

	for (uint32_t sample = 0u; sample < framebuffer->get_sample_count(); ++sample)
	{
		FRay ray{ ray_origin, ray_direction};
		framebuffer->add_pixel(x, y, glm::vec4(hit_pixel(engine, ray, 100), 1.f));
	}
}

glm::vec3 CRenderSystem::hit_pixel(CRayEngine* engine, FRay ray, int32_t bounces)
{
	auto& scene = engine->get_scene();
	auto& resource_manager = engine->get_resource_manager();

	if (bounces <= 0)
		return glm::vec3(0.f);

	FHitResult hit_result{};
	if(!scene->trace_ray(ray, 0.001f, std::numeric_limits<float>::infinity(), hit_result))
		return ray_color(ray);

	auto& material = resource_manager->get_material(hit_result.m_material_id);

	glm::vec3 attenuation{};
	glm::vec3 emitted = material->emit(hit_result);

	if (!material->scatter(ray, hit_result, attenuation, ray))
		return emitted;

	return emitted + attenuation * hit_pixel(engine, ray, bounces - 1);
}