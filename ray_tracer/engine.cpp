#include "engine.h"

#include "ecs/systems/systems.h"

#include <configuration.h>

void CRayEngine::create()
{
	auto& config = CConfiguration::getInstance()->get();

	m_pResourceManager = std::make_unique<CResourceManager>();

	m_pRenderer = std::make_unique<CIntegrator>(m_pResourceManager.get());
	m_pRenderer->create(config.m_fbcfg.m_width, config.m_fbcfg.m_height, config.m_icfg.m_sample_count, config.m_icfg.m_bounce_count, config.m_icfg.m_rr_threshold);

	m_pScene = std::make_unique<CScene>(m_pResourceManager.get());
	
	m_pScene->create(config.m_scfg.m_scene_path);

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