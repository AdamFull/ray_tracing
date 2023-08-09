#include "renderer.h"
#include "resources/resource_manager.h"

#include "ecs/components/camera_component.h"

#include "util.h"

glm::vec3 ray_color(const FRay& ray)
{
	auto direction = glm::normalize(ray.m_direction);
	auto t = 0.5f * (direction.y + 1.0f);
	return glm::vec3(glm::vec3(1.f, 1.f, 1.f) * (1.f - t) + glm::vec3(0.5f, 0.7f, 1.f) * t); // day
	//return glm::vec3(glm::vec3(0.01f, 0.01f, 0.03f) * (1.f - t) + glm::vec3(0.0f, 0.0f, 0.0f) * t); // night
}

CRenderCore::CRenderCore(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CRenderCore::create(uint32_t width, uint32_t heigth, uint32_t samples, uint32_t bounces)
{
	// Creating a framebuffer
	m_pFramebuffer = std::make_unique<CFramebuffer>(m_pResourceManager);
	m_pFramebuffer->create(width, heigth);

	m_sampleCount = samples;
	m_bounceCount = bounces;

	m_pixelIterator.resize(width * heigth);
	std::iota(m_pixelIterator.begin(), m_pixelIterator.end(), 0u);
}

void CRenderCore::trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin)
{
	for (uint32_t sample = 1u; sample <= m_sampleCount; ++sample)
	{
		auto start = std::chrono::high_resolution_clock::now();

		std::for_each(std::execution::par_unseq, m_pixelIterator.begin(), m_pixelIterator.end(),
			[this, scene, camera, &origin](uint32_t index)
			{
				trace_ray(scene, camera, origin, index);
			});

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		std::cout << std::format("Frame {}: time {} ms\n", sample, duration);

		m_pFramebuffer->increment_sample_count();

		if (sample % 5u == 0u)
		{
			m_pFramebuffer->present();
			
			auto image_id = m_pFramebuffer->get_image();
			auto& image = m_pResourceManager->get_image(image_id);
			image->save(std::format("image_{}s.png", sample));
		}
	}
}

void CRenderCore::trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin, uint32_t ray_index)
{
	auto& viewport_extent = camera->m_viewportExtent;

	auto x = ray_index % viewport_extent.x;
	auto y = ray_index / viewport_extent.x;

	auto& ray_direction = camera->m_vRayDirections[ray_index];

	FRay ray{};
	ray.m_origin = origin;
	ray.set_direction(ray_direction + random_vec3(-0.0001f, 0.0001f));
	m_pFramebuffer->add_pixel(x, y, glm::vec4(hit_pixel(scene, ray, m_bounceCount), 1.f));
}

glm::vec3 CRenderCore::hit_pixel(CScene* scene, FRay ray, int32_t bounces)
{
	if (bounces <= 0)
		return glm::vec3(0.f);

	FHitResult hit_result{};
	if (!scene->trace_ray(ray, 0.001f, std::numeric_limits<float>::infinity(), hit_result))
		return ray_color(ray);

	auto& material = m_pResourceManager->get_material(hit_result.m_material_id);

	float pdf{};
	glm::vec3 attenuation{};
	glm::vec3 emitted = material->emit(hit_result);

	FRay scattered{};
	if (!material->scatter(ray, hit_result, attenuation, scattered, pdf))
		return emitted;

	//return emitted + attenuation * hit_pixel(scene, scattered, bounces - 1);
	return emitted + attenuation * hit_pixel(scene, scattered, bounces - 1);
}

const std::unique_ptr<CFramebuffer>& CRenderCore::get_framebuffer() const
{
	return m_pFramebuffer;
}

const std::vector<uint32_t>& CRenderCore::get_pixel_iterator() const
{
	return m_pixelIterator;
}