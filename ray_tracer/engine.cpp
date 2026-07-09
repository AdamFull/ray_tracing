#include "engine.h"

#include "ecs/systems/systems.h"
#include "ecs/components/camera_component.h"
#include "ecs/components/transform_component.h"

#include <configuration.h>

#include <logger/logger.h>

void CRayEngine::create()
{
	auto& config = CConfiguration::getInstance()->get();

	m_pResourceManager = std::make_unique<CResourceManager>();

	m_pScene = std::make_unique<CScene>(m_pResourceManager.get());
	m_pScene->create(config.m_scfg.m_scene_path);

	auto& registry = m_pScene->get_registry();

	FCameraComponent* current_camera{ nullptr };
	auto view = registry.view<FCameraComponent>();
	for (auto [entity, camera] : view.each())
	{
		current_camera = &camera;
		break;
	}

	// If the scene provides no camera, synthesise a default one. It is framed to fit the
	// whole scene once the acceleration structure (and therefore the scene bounds) is built.
	entt::entity default_camera{ entt::null };
	if (!current_camera)
	{
		default_camera = registry.create();
		registry.emplace<FTransformComponent>(default_camera, FTransformComponent{});

		FCameraComponent default_component{};
		// Match the requested output aspect so the framebuffer isn't distorted.
		default_component.m_aspect = static_cast<float>(config.m_fbcfg.m_width) / static_cast<float>(config.m_fbcfg.m_height);
		registry.emplace<FCameraComponent>(default_camera, default_component);

		current_camera = registry.try_get<FCameraComponent>(default_camera);
		log_info("No camera found in the scene. A default framing camera was created.");
	}

	uint32_t height = config.m_fbcfg.m_height;
	uint32_t width = current_camera->m_aspect * height;

	m_pRenderer = std::make_unique<CIntegrator>(m_pResourceManager.get());
	m_pRenderer->create(width, height, config.m_icfg.m_sample_count);

	// Create systems
	m_vSystems.emplace_back(std::make_unique<CHierarchySystem>());
	m_vSystems.emplace_back(std::make_unique<CCameraSystem>());
	m_vSystems.emplace_back(std::make_unique<CRenderSystem>());

	for (auto& system : m_vSystems)
		system->create(this);

	// The render system built the BVH, so scene bounds are now available: place the default
	// camera to look at the scene centre from far enough back to frame the bounding sphere.
	if (registry.valid(default_camera))
	{
		auto bounds = m_pScene->get_bounds();
		glm::vec3 center = 0.5f * (bounds.m_min + bounds.m_max);
		float radius = 0.5f * glm::length(bounds.extent());
		if (!(radius > 0.f))
			radius = 1.f;

		float fovy = glm::radians(current_camera->m_fov);
		float distance = radius / glm::tan(fovy * 0.5f) * 1.3f;

		auto& transform = registry.get<FTransformComponent>(default_camera);
		glm::vec3 eye = center + glm::vec3(0.f, 0.f, distance);

		transform.m_position = eye;
		transform.m_position_g = eye;
		transform.m_rotation = glm::quat(1.f, 0.f, 0.f, 0.f);
		transform.m_rotation_g = glm::quat(1.f, 0.f, 0.f, 0.f);
		current_camera->m_bWasMoved = true;
	}
}

void CRayEngine::update()
{
	for (auto& system : m_vSystems)
		system->update(this);
}

const std::unique_ptr<CResourceManager>& CRayEngine::get_resource_manager() const
{
	return m_pResourceManager;
}

const std::unique_ptr<CIntegrator>& CRayEngine::get_renderer() const
{
	return m_pRenderer;
}

const std::unique_ptr<CScene>& CRayEngine::get_scene() const
{
	return m_pScene;
}