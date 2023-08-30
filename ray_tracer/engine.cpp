#include "engine.h"

#include "ecs/systems/systems.h"
#include "ecs/components/camera_component.h"

#include <configuration.h>

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

	uint32_t height = config.m_fbcfg.m_height;
	uint32_t width = current_camera->m_aspect * height;

	m_pRenderer = std::make_unique<CIntegrator>(m_pResourceManager.get());
	m_pRenderer->create(width, height, config.m_icfg.m_sample_count, config.m_icfg.m_bounce_count, config.m_icfg.m_rr_threshold);

	// Create systems
	m_vSystems.emplace_back(std::make_unique<CHierarchySystem>());
	m_vSystems.emplace_back(std::make_unique<CCameraSystem>());
	m_vSystems.emplace_back(std::make_unique<CRenderSystem>());

	for (auto& system : m_vSystems)
		system->create(this);
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