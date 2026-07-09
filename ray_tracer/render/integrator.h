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

	void create(uint32_t width, uint32_t heigth, uint32_t samples);

	void trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin);

	// Interactive preview: trace a single sample per pixel and accumulate it into the colour
	// attachment. Call repeatedly with an increasing frame_index for progressive refinement;
	// clear the framebuffer colour attachment to restart accumulation (e.g. on camera move).
	void render_preview(CScene* scene, FCameraComponent* camera, const glm::vec3& origin, uint32_t frame_index, uint32_t bounces);

	const std::unique_ptr<CFramebuffer>& get_framebuffer() const;
	const std::vector<uint32_t>& get_pixel_iterator() const;
private:
	void trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin, uint32_t ray_index);
	glm::vec3 integrate_nee(CScene* scene, FRay ray, int32_t bounces, const std::unique_ptr<CSamplerBase>& sampler, glm::vec3& surface_albedo, glm::vec3& surface_normal);
	glm::vec3 integrate(CScene* scene, FRay ray, int32_t bounces, const std::unique_ptr<CSamplerBase>& sampler, glm::vec3& surface_albedo, glm::vec3& surface_normal);
private:
	CResourceManager* m_pResourceManager{ nullptr };
	std::unique_ptr<CFramebuffer> m_pFramebuffer{};

	glm::vec3 m_sky_begin{};
	glm::vec3 m_sky_end{};

	uint32_t m_sampleCount{ 1u };
	uint32_t m_bounceCount{ 1u };
	uint32_t m_rrThreshold{ 3u };

	// Estimator
	bool m_use_estimator{ false };
	float m_estimator_tolerance{ 0.05f };

	std::vector<uint32_t> m_pixelIterator{};
};