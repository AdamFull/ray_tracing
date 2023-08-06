#include "render_system.h"

#include "engine.h"
#include "util.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/camera_component.h"
#include "ecs/components/mesh_component.h"

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

	math::vec3 origin{ camera_transform->m_position.x, camera_transform->m_position.y, camera_transform->m_position.z };
	renderer->trace_ray(scene.get(), current_camera, origin);
}