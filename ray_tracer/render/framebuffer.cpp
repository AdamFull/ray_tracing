#include "renderer.h"

#include "resources/resource_manager.h"

glm::vec3 linear_to_srgb(const glm::vec3& linearRGB)
{
	auto cutoff = glm::lessThan(linearRGB, glm::vec3(0.0031308f));
	glm::vec3 higher = glm::vec3(1.055f) * glm::pow(linearRGB, glm::vec3(1.f / 2.4f)) - glm::vec3(0.055f);
	glm::vec3 lower = linearRGB * glm::vec3(12.92f);

	return glm::mix(higher, lower, cutoff);
}

glm::vec3 convertXYZ2Yxy(const glm::vec3& _xyz)
{
	// Reference(s):
	// - XYZ to xyY
	//   https://web.archive.org/web/20191027010144/http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_xyY.html
	float inv = 1.f / glm::dot(_xyz, glm::vec3(1.f));
	return glm::vec3(_xyz.y, _xyz.x * inv, _xyz.y * inv);
}

glm::vec3 convertXYZ2RGB(const glm::vec3& _xyz)
{
	glm::vec3 rgb;
	rgb.x = glm::dot(glm::vec3(3.2404542f, -1.5371385f, -0.4985314f), _xyz);
	rgb.y = glm::dot(glm::vec3(-0.9692660f, 1.8760108f, 0.0415560f), _xyz);
	rgb.z = glm::dot(glm::vec3(0.0556434f, -0.2040259f, 1.0572252f), _xyz);
	return rgb;
}

glm::vec3 convertRGB2XYZ(const glm::vec3& _rgb)
{
	// Reference(s):
	// - RGB/XYZ Matrices
	//   https://web.archive.org/web/20191027010220/http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
	glm::vec3 xyz;
	xyz.x = glm::dot(glm::vec3(0.4124564f, 0.3575761f, 0.1804375f), _rgb);
	xyz.y = glm::dot(glm::vec3(0.2126729f, 0.7151522f, 0.0721750f), _rgb);
	xyz.z = glm::dot(glm::vec3(0.0193339f, 0.1191920f, 0.9503041f), _rgb);
	return xyz;
}

glm::vec3 convertYxy2XYZ(const glm::vec3& _Yxy)
{
	// Reference(s):
	// - xyY to XYZ
	//   https://web.archive.org/web/20191027010036/http://www.brucelindbloom.com/index.html?Eqn_xyY_to_XYZ.html
	glm::vec3 xyz;
	xyz.x = _Yxy.x * _Yxy.y / _Yxy.z;
	xyz.y = _Yxy.x;
	xyz.z = _Yxy.x * (1.f - _Yxy.y - _Yxy.z) / _Yxy.z;
	return xyz;
}

glm::vec3 convertRGB2Yxy(const glm::vec3& _rgb)
{
	return convertXYZ2Yxy(convertRGB2XYZ(_rgb));
}

glm::vec3 convertYxy2RGB(const glm::vec3& _Yxy)
{
	return convertXYZ2RGB(convertYxy2XYZ(_Yxy));
}

glm::vec3 to_gamma(const glm::vec3& _rgb, float gamma)
{
	return glm::pow(glm::abs(_rgb), glm::vec3(1.f / gamma));
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
			auto fragcolor = get_pixel(x, y);
			auto srgb = glm::vec3(fragcolor);

			glm::vec3 Yxy = convertRGB2Yxy(srgb);
			Yxy.x = Yxy.x * glm::pow(2.f, 2.7f);

			srgb = convertYxy2RGB(Yxy);
			srgb = to_gamma(srgb, 2.2f);

			image->set_pixel(x, m_dimensions.y - y - 1u, pack_rgba(glm::vec4(linear_to_srgb(srgb), fragcolor.a)));
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