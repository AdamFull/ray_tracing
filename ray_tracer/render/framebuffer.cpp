#include "renderer.h"

#include "resources/resource_manager.h"

CFramebuffer::CFramebuffer(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CFramebuffer::create(uint32_t width, uint32_t heigth)
{
	// Creating framebuffer image
	m_imageId = m_pResourceManager->add_image("framebuffer_image", std::make_unique<CImage>(width, heigth));

	m_dimensions = glm::uvec2(width, heigth);

	m_pColor = std::make_unique<glm::vec4[]>(width * heigth);
}

void CFramebuffer::increment_sample_count()
{
	m_uSamples++;
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
			auto sampling_factor = 1.f / static_cast<float>(m_uSamples);
			auto color = get_pixel(x, y) * sampling_factor;
			color = glm::sqrt(color);
			image->set_pixel(x, m_dimensions.y - y - 1u, pack_color_u32(color));
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