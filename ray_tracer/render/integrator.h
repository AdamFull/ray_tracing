#pragma once

#include "scene.h"
#include "framebuffer.h"
#include "shared.h"
#include "rsampler.h"

class CResourceManager;

class CIntegrator
{
public:
	CIntegrator() = default;
	CIntegrator(CResourceManager* resource_manager);

	void create(uint32_t width, uint32_t heigth, uint32_t samples, uint32_t bounces, uint32_t rr_threshold);

	void trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin);

	const std::unique_ptr<CFramebuffer>& get_framebuffer() const;
	const std::vector<uint32_t>& get_pixel_iterator() const;
private:
	void trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin, uint32_t ray_index);
	glm::vec3 integrate(CScene* scene, FRay ray, int32_t bounces, CCMGSampler& sampler);
private:
	CResourceManager* m_pResourceManager{ nullptr };
	std::unique_ptr<CFramebuffer> m_pFramebuffer{};

	uint32_t m_sampleCount{ 1u };
	uint32_t m_bounceCount{ 1u };
	uint32_t m_rrThreshold{ 3u };
	std::vector<uint32_t> m_pixelIterator{};
};