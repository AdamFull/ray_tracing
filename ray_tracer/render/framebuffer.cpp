#include "integrator.h"

#include "resources/resource_manager.h"

#include "tonemapping.hpp"
#include "configuration.h"

#include <logger/logger.h>

//#include <OpenImageDenoise/oidn.hpp>

//#define UNCHARTED_TONEMAP
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

void CFramebuffer::create(uint32_t width, uint32_t heigth, uint32_t attachment_mask)
{
	auto& config = CConfiguration::getInstance()->get();

	// Creating framebuffer image
	m_imageId = m_pResourceManager->add_image("framebuffer_image", std::make_unique<CImage>(width, heigth));

	m_dimensions = glm::uvec2(width, heigth);

	if(attachment_mask & FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT)
		m_colorBuffers[FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT] = std::make_unique<glm::vec3[]>(width * heigth);
	if (attachment_mask & FRAMEBUFFER_ALBEDO_ATTACHMENT_FLAG_BIT)
		m_colorBuffers[FRAMEBUFFER_ALBEDO_ATTACHMENT_FLAG_BIT] = std::make_unique<glm::vec3[]>(width * heigth);
	if (attachment_mask & FRAMEBUFFER_NORMAL_ATTACHMENT_FLAG_BIT)
		m_colorBuffers[FRAMEBUFFER_NORMAL_ATTACHMENT_FLAG_BIT] = std::make_unique<glm::vec3[]>(width * heigth);

	m_gamma = config.m_tmcfg.m_gamma;
	m_exposure = config.m_tmcfg.m_exposure;
}

const glm::vec3& CFramebuffer::get_pixel(uint32_t x, uint32_t y, uint32_t attachment_mask)
{
	return m_colorBuffers[attachment_mask][y * m_dimensions.x + x];
}

void CFramebuffer::set_pixel(uint32_t x, uint32_t y, const glm::vec3& color, uint32_t attachment_mask)
{
	m_colorBuffers[attachment_mask][y * m_dimensions.x + x] = color;
}

void CFramebuffer::add_pixel(uint32_t x, uint32_t y, const glm::vec3& color, uint32_t attachment_mask)
{
	m_colorBuffers[attachment_mask][y * m_dimensions.x + x] += color;
}

void CFramebuffer::clear(const glm::vec3& clear_color, uint32_t attachment_mask)
{
	auto size = m_dimensions.x * m_dimensions.y;
	for (uint32_t idx = 0u; idx < size; ++idx)
		m_colorBuffers[attachment_mask][idx] = clear_color;
}

void CFramebuffer::present()
{
	auto& image = m_pResourceManager->get_image(m_imageId);

	auto& color_buffer = m_colorBuffers[FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT];
	//auto& albedo_buffer = m_colorBuffers[FRAMEBUFFER_ALBEDO_ATTACHMENT_FLAG_BIT];
	//auto& normal_buffer = m_colorBuffers[FRAMEBUFFER_NORMAL_ATTACHMENT_FLAG_BIT];
	//std::unique_ptr<glm::vec3[]> output_buffer = std::make_unique<glm::vec3[]>(m_dimensions.x * m_dimensions.y);
	//
	//oidn::DeviceRef device = oidn::newDevice();
	//device.commit();
	//
	//oidn::FilterRef filter = device.newFilter("RT");
	//filter.setImage("color", color_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y);
	//filter.setImage("albedo", albedo_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y); // auxiliary
	//filter.setImage("normal", normal_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y); // auxiliary
	//filter.setImage("output", output_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y);
	//filter.set("hdr", true); // beauty image is HDR
	//filter.set("cleanAux", true); // auxiliary images will be prefiltered
	//filter.commit();
	//
	//// Create a separate filter for denoising an auxiliary albedo image (in-place)
	//oidn::FilterRef albedoFilter = device.newFilter("RT"); // same filter type as for beauty
	//albedoFilter.setImage("albedo", albedo_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y);
	//albedoFilter.setImage("output", albedo_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y);
	//albedoFilter.commit();
	//
	//// Create a separate filter for denoising an auxiliary normal image (in-place)
	//oidn::FilterRef normalFilter = device.newFilter("RT"); // same filter type as for beauty
	//normalFilter.setImage("normal", normal_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y);
	//normalFilter.setImage("output", normal_buffer.get(), oidn::Format::Float3, m_dimensions.x, m_dimensions.y);
	//normalFilter.commit();
	//
	//// Prefilter the auxiliary images
	//albedoFilter.execute();
	//normalFilter.execute();
	//
	//// Filter the beauty image
	//filter.execute();

	//const char* errorMessage;
	//if (device.getError(errorMessage) != oidn::Error::None) 
	//{
	//	log_error("OIDN error: {}", errorMessage);
	//	return;
	//}

	for (uint32_t y = 0u; y < m_dimensions.y; ++y)
	{
		for (uint32_t x = 0u; x < m_dimensions.x; ++x)
		{
			glm::vec3 color = color_buffer[y * m_dimensions.x + x];

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
			//image->set_pixel(x, m_dimensions.y - y - 1u, glm::vec4(linear_to_srgb(color), fragcolor.a));
			image->set_pixel(x, m_dimensions.y - y - 1u, glm::vec4(ACESTonemap(color), 1.f));
			//image->set_pixel(x, m_dimensions.y - y - 1u, glm::vec4(color, 1.f));
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