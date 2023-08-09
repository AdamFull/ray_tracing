#pragma once

#include "scene.h"
#include "render/renderer.h"
#include "resources/resource_manager.h"
#include "ecs/systems/base_system.h"

class CRayEngine
{
public:
	void create(const std::filesystem::path& scenepath, uint32_t width, uint32_t height, uint32_t sample_count, uint32_t bounce_count);

	void update();

	const std::unique_ptr<CResourceManager>& get_resource_manager() const;
	const std::unique_ptr<CRenderCore>& get_renderer() const;
	const std::unique_ptr<CScene>& get_scene() const;
private:
	resource_id_t generate_brdf_lut(uint32_t size, uint32_t samples);
private:
	std::vector<std::unique_ptr<CBaseSystem>> m_vSystems{};

	std::unique_ptr<CResourceManager> m_pResourceManager{};
	std::unique_ptr<CRenderCore> m_pRenderer{};
	std::unique_ptr<CScene> m_pScene{};
};