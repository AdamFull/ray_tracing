#include "renderer.h"

#include "resources/resource_manager.h"

uint32_t pack_color_u32(const glm::vec4& color)
{
	uint32_t r = static_cast<uint32_t>(glm::clamp(color.x, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t g = static_cast<uint32_t>(glm::clamp(color.y, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t b = static_cast<uint32_t>(glm::clamp(color.z, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t a = static_cast<uint32_t>(glm::clamp(color.w, 0.f, 1.f) * 255.f) & 0xFFu;

	return (a << 24) | (b << 16) | (g << 8) | r;
}


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

void CFramebuffer::set_sample_count(uint32_t sample_count)
{
	m_uSamples = sample_count ? sample_count : 1u;
}

uint32_t CFramebuffer::get_sample_count() const
{
	return m_uSamples;
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
			image->set_pixel(x, m_dimensions.y - y - 1u, pack_color_u32(glm::sqrt(get_pixel(x, y) * sampling_factor)));
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