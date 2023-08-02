#include "engine.h"

#include "ecs/systems/systems.h"

void CRayEngine::create(const std::filesystem::path& scenepath, uint32_t width, uint32_t heigth)
{
	m_pResourceManager = std::make_unique<CResourceManager>();

	m_pRenderer = std::make_unique<CRenderCore>(m_pResourceManager.get());
	m_pRenderer->create(width, heigth);

	m_pScene = std::make_unique<CScene>(m_pResourceManager.get());
	m_pScene->create(scenepath);

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

const std::unique_ptr<CRenderCore>& CRayEngine::get_renderer() const
{
	return m_pRenderer;
}

const std::unique_ptr<CScene>& CRayEngine::get_scene() const
{
	return m_pScene;
}