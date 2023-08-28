#include "integrator.h"

#include "resources/resource_manager.h"

#include "tonemapping.hpp"
#include "configuration.h"

#define UNCHARTED_TONEMAP
//#define ANOTHER_TONEMAP

inline glm::vec3 linear_to_srgb(const glm::vec3& linearRGB)
{
	auto cutoff = glm::lessThan(linearRGB, glm::vec3(0.0031308f));
	glm::vec3 higher = glm::vec3(1.055f) * glm::pow(linearRGB, glm::vec3(1.f / 2.4f)) - glm::vec3(0.055f);
	glm::vec3 lower = linearRGB * glm::vec3(12.92f);

	return glm::mix(higher, lower, cutoff);
}

CFramebuffer::CFramebuffer(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CFramebuffer::create(uint32_t width, uint32_t heigth)
{
	auto& config = CConfiguration::getInstance()->get();

	// Creating framebuffer image
	m_imageId = m_pResourceManager->add_image("framebuffer_image", std::make_unique<CImage>(width, heigth));

	m_dimensions = glm::uvec2(width, heigth);

	m_pColor = std::make_unique<glm::vec4[]>(width * heigth);

	m_gamma = config.m_tmcfg.m_gamma;
	m_exposure = config.m_tmcfg.m_exposure;
}

const glm::vec4& CFramebuffer::get_pixel(uint32_t x, uint32_t y)
{
	return m_pColor[y * m_dimensions.x + x];
}

void CFramebuffer::set_pixel(uint32_t x, uint32_t y, const glm::vec4& color)
{
	m_pColor[y * m_dimensions.x + x] = color;
}

void CFramebuffer::add_pixel(uint32_t x, uint32_t y, const glm::vec4& color)
{
	m_pColor[y * m_dimensions.x + x] += color;
}

void CFramebuffer::clear(const glm::vec4& clear_color)
{
	auto size = m_dimensions.x * m_dimensions.y;
	for (uint32_t idx = 0u; idx < size; ++idx)
		m_pColor[idx] = clear_color;
}

void CFramebuffer::present()
{
	auto& image = m_pResourceManager->get_image(m_imageId);

	for (uint32_t y = 0u; y < m_dimensions.y; ++y)
	{
		for (uint32_t x = 0u; x < m_dimensions.x; ++x)
		{
			glm::vec4 fragcolor = get_pixel(x, y);
			glm::vec3 color = glm::vec3(fragcolor);

#if defined(UNCHARTED_TONEMAP)
			glm::vec3 curr = Uncharted2Tonemap(color * m_exposure * 2.f);
			auto white_scale = 1.f / Uncharted2Tonemap(glm::vec3(11.2f));
			color = curr * white_scale;
			color = apply_gamma(color, m_gamma);
#elif defined(ANOTHER_TONEMAP)
			glm::vec3 Yxy = convertRGB2Yxy(color);
			Yxy.x = Yxy.x * glm::pow(2.f, m_exposure);
			color = convertYxy2RGB(Yxy);
			color = apply_gamma(color, m_gamma);
#endif
			image->set_pixel(x, m_dimensions.y - y - 1u, pack_rgba(glm::vec4(linear_to_srgb(color), fragcolor.a)));
		}
	}
}

resource_id_t CFramebuffer::get_image() const
{
	return m_imageId;
}

const glm::uvec2& CFramebuffer::get_extent() const
{
	return m_dimensions;
}