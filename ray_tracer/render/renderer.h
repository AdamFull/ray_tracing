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

	void create(uint32_t width, uint32_t heigth, uint32_t samples);

	void trace_ray(CScene* scene, FCameraComponent* camera, const math::vec3& origin);

	const std::unique_ptr<CFramebuffer>& get_framebuffer() const;
	const std::vector<uint32_t>& get_pixel_iterator() const;
private:
	void trace_ray(CScene* scene, FCameraComponent* camera, const math::vec3& origin, uint32_t ray_index);
	math::vec3 hit_pixel(CScene* scene, FRay ray, int32_t bounces);
private:
	CResourceManager* m_pResourceManager{ nullptr };
	std::unique_ptr<CFramebuffer> m_pFramebuffer{};

	std::vector<uint32_t> m_pixelIterator{};
};