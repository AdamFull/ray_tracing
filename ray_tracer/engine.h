#pragma once

#include "scene.h"
#include "render/integrator.h"
#include "resources/resource_manager.h"
#include "ecs/systems/base_system.h"

class CRayEngine
{
public:
	void create();

	void update();

	const std::unique_ptr<CResourceManager>& get_resource_manager() const;
	const std::unique_ptr<CIntegrator>& get_renderer() const;
	const std::unique_ptr<CScene>& get_scene() const;
private:
	std::vector<std::unique_ptr<CBaseSystem>> m_vSystems{};

	std::unique_ptr<CResourceManager> m_pResourceManager{};
	std::unique_ptr<CIntegrator> m_pRenderer{};
	std::unique_ptr<CScene> m_pScene{};
};