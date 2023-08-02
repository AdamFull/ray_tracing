#include "renderer.h"
#include "resources/resource_manager.h"

#include "util.h"

#include <numeric>

CRenderCore::CRenderCore(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CRenderCore::create(const uint32_t width, const uint32_t heigth)
{
	// Creating a framebuffer
	m_pFramebuffer = std::make_unique<CFramebuffer>(m_pResourceManager);
	m_pFramebuffer->create(width, heigth);
	m_pFramebuffer->set_sample_count(100);

	m_pixelIterator.resize(width * heigth);
	std::iota(m_pixelIterator.begin(), m_pixelIterator.end(), 0u);
}

const std::unique_ptr<CFramebuffer>& CRenderCore::get_framebuffer() const
{
	return m_pFramebuffer;
}

const std::vector<uint32_t>& CRenderCore::get_pixel_iterator() const
{
	return m_pixelIterator;
}

//void CRenderCore::render(const std::unique_ptr<Scene>& scene)
//{
//	auto dim = _framebuffer->get_extent();
//
//	_framebuffer->clear(glm::vec4(0.f));
//
//	std::for_each(std::execution::par, _image_iter.begin(), _image_iter.end(),
//		[this, &scene, &dim](uint32_t index)
//		{
//			auto x = index % dim.x;
//			auto y = index / dim.x;
//	
//			for (uint32_t sample = 0u; sample < _framebuffer->get_sample_count(); ++sample)
//			{
//				float u = (static_cast<float>(x) + random<float>()) / static_cast<float>(dim.x - 1u);
//				float v = (static_cast<float>(y) + random<float>()) / static_cast<float>(dim.y - 1u);
//	
//				auto ray = _camera->get_ray(u, v);
//				_framebuffer->add_pixel(x, y, glm::vec4(trace_ray(ray, scene, 50), 1.f));
//			}
//		});
//
//	_framebuffer->present();
//}
//
//glm::vec3 CRenderCore::trace_ray(Ray ray, const std::unique_ptr<Scene>& scene, int32_t bounces)
//{
//	glm::vec3 out_color{ 1.f };
//
//	for (uint32_t b = 0u; b < bounces; ++b)
//	{
//		HitResult hit_result{};
//		if (!scene->trace_ray(ray, 0.0005f, std::numeric_limits<float>::infinity(), hit_result))
//		{
//			out_color *= ray_color(ray);
//			break;
//		}
//
//		auto& material = scene->get_material(hit_result._material_id);
//		if (!material)
//			continue;
//
//		material->scatter(ray, hit_result, out_color, ray);
//	}
//	
//	return out_color;
//}
//
//void CRenderCore::render_loop()
//{
//
//}