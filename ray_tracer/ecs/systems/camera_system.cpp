#include "camera_system.h"

#include "engine.h"
#include "configuration.h"

#include "ecs/components/transform_component.h"
#include "ecs/components/hierarchy_component.h"
#include "ecs/components/camera_component.h"

void CCameraSystem::create(CRayEngine* engine)
{

}

void CCameraSystem::update(CRayEngine* engine)
{
	auto& config = CConfiguration::getInstance()->get();
	auto& renderer = engine->get_renderer();
	auto& framebuffer = renderer->get_framebuffer();
	auto& extent = framebuffer->get_extent();

	auto& scene = engine->get_scene();
	auto& registry = scene->get_registry();

	if (!config.m_scfg.m_camera_name.empty())
	{
		auto view = registry.view<FTransformComponent, FHierarchyComponent, FCameraComponent>();
		for (auto [entity, transform, hierarchy, camera] : view.each())
		{
			if(config.m_scfg.m_camera_name == hierarchy.m_name)
				update_camera(registry, extent, &camera, &transform);
		}
	}
	else
	{
		size_t camera_idx{ 0ull };
		auto view = registry.view<FTransformComponent, FCameraComponent>();
		for (auto [entity, transform, camera] : view.each())
		{
			if (config.m_scfg.m_camera_id == camera_idx)
				update_camera(registry, extent, &camera, &transform);
			++camera_idx;
		}
	}
		
}

void CCameraSystem::update_camera(entt::registry& registry, const glm::uvec2& extent, FCameraComponent* camera, FTransformComponent* transform)
{
	bool needToRecalculateRays{ false };

	camera->m_forward = glm::normalize(glm::rotate(transform->m_rotation_g, glm::vec3(0.f, 0.f, -1.f)));
	camera->m_right = glm::normalize(glm::cross(camera->m_forward, glm::vec3{ 0.f, 1.f, 0.f }));
	camera->m_up = glm::normalize(glm::cross(camera->m_right, camera->m_forward));

	if (extent.x != camera->m_viewportExtent.x || extent.y != camera->m_viewportExtent.y)
	{
		camera->m_viewportExtent.x = camera->m_aspect * extent.y;
		camera->m_viewportExtent.y = extent.y;

		recalculate_projection(camera);
		needToRecalculateRays = true;
	}

	if (camera->m_bWasMoved)
	{
		recalculate_view(camera, transform);
		camera->m_bWasMoved = false;
		needToRecalculateRays = true;
	}

	if (needToRecalculateRays)
		recalculate_ray_directions(camera);
}

void CCameraSystem::recalculate_projection(FCameraComponent* camera)
{
	if (camera->m_type == ECameraType::eOrthographic)
		camera->m_projection = glm::ortho(camera->m_xmag, camera->m_ymag, 0.f, 1.f, camera->m_near, camera->m_far);
	else
		camera->m_projection = glm::perspective(camera->m_fov, camera->m_aspect, camera->m_near, camera->m_far);

	camera->m_invProjection = glm::inverse(camera->m_projection);
}

void CCameraSystem::recalculate_view(FCameraComponent* camera, FTransformComponent* transform)
{
	camera->m_view = glm::lookAt(transform->m_position_g, transform->m_position_g + camera->m_forward, camera->m_up);
	camera->m_invView = glm::inverse(camera->m_view);
}

void CCameraSystem::recalculate_ray_directions(FCameraComponent* camera)
{
	auto& viewport_extent = camera->m_viewportExtent;
	camera->m_vRayDirections.resize(viewport_extent.x * viewport_extent.y);

	for (uint32_t y = 0; y < viewport_extent.y; y++)
	{
		for (uint32_t x = 0; x < viewport_extent.x; x++)
		{
			glm::vec2 texcoord = { static_cast<float>(x) / static_cast<float>(viewport_extent.x), static_cast<float>(y) / static_cast<float>(viewport_extent.y) };
			texcoord = texcoord * 2.f - 1.f;

			glm::vec4 target = camera->m_invProjection * glm::vec4(texcoord.x, texcoord.y, 1.f, 1.f);
			auto dir = glm::vec3(camera->m_invView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0.f));
			camera->m_vRayDirections[x + y * viewport_extent.x] = glm::vec3(dir.x, dir.y, dir.z);
		}
	}
}