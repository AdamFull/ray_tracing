#pragma once

#include "scene.h"
#include "framebuffer.h"
#include "shared.h"

class CResourceManager;

class CRenderCore
{
public:
	CRenderCore() = default;
	CRenderCore(CResourceManager* resource_manager);

	void create(const uint32_t width, const uint32_t heigth);

	const std::unique_ptr<CFramebuffer>& get_framebuffer() const;
	const std::vector<uint32_t>& get_pixel_iterator() const;

	//void render(const std::unique_ptr<Scene>& scene);
	//glm::vec3 trace_ray(Ray ray, const std::unique_ptr<Scene>& scene, int32_t bounces);
	//
protected:
	//void render_loop();
private:
	CResourceManager* m_pResourceManager{ nullptr };
	std::unique_ptr<CFramebuffer> m_pFramebuffer{};

	std::vector<uint32_t> m_pixelIterator{};
};