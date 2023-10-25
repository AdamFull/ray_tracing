#include "render_system.h"

#include "engine.h"
#include "util.h"
#include "configuration.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/hierarchy_component.h"
#include "ecs/components/camera_component.h"
#include "ecs/components/mesh_component.h"

void CRenderSystem::create(CRayEngine* engine)
{
	auto& scene = engine->get_scene();
	scene->build_acceleration();
}

void CRenderSystem::update(CRayEngine* engine)
{
	auto& config = CConfiguration::getInstance()->get();
	auto& renderer = engine->get_renderer();
	auto& pixel_iter = renderer->get_pixel_iterator();

	auto& scene = engine->get_scene();
	auto& registry = scene->get_registry();

	FCameraComponent* current_camera{ nullptr };
	FTransformComponent* camera_transform{ nullptr };

	if (!config.m_scfg.m_camera_name.empty())
	{
		auto view = registry.view<FTransformComponent, FHierarchyComponent, FCameraComponent>();
		for (auto [entity, transform, hierarchy, camera] : view.each())
		{
			if (config.m_scfg.m_camera_name == hierarchy.m_name)
			{
				current_camera = &camera;
				camera_transform = &transform;
				break;
			}
		}
	}
	else
	{
		size_t camera_idx{ 0ull };
		auto view = registry.view<FTransformComponent, FCameraComponent>();
		for (auto [entity, transform, camera] : view.each())
		{
			if (config.m_scfg.m_camera_id == camera_idx)
			{
				current_camera = &camera;
				camera_transform = &transform;
				break;
			}
			++camera_idx;
		}
	}

	if (!current_camera || !camera_transform)
		return;

	renderer->trace_ray(scene.get(), current_camera, camera_transform->m_position_g);
}