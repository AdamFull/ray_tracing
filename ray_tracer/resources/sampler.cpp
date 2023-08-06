#include "resource_manager.h"

math::vec4 unpack_rgba(uint32_t packed_color)
{
	float a = static_cast<float>((packed_color >> 24u) & 0xFFu) / 255.0f;
	float b = static_cast<float>((packed_color >> 16u) & 0xFFu) / 255.0f;
	float g = static_cast<float>((packed_color >> 8u) & 0xFFu) / 255.0f;
	float r = static_cast<float>(packed_color & 0xFFu) / 255.0f;

	return math::vec4(r, g, b, a);
}

CSampler::CSampler(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CSampler::create(EFilterMode filterMin, EFilterMode filterMag, EWrapMode wrapS, EWrapMode wrapT)
{
	m_minFilter = filterMin;
	m_magFilter = filterMag;
	m_wrapS = wrapS;
	m_wrapT = wrapT;
}

math::vec4 CSampler::sample(resource_id_t image_id, const math::vec2& uv)
{
	auto& image = m_pResourceManager->get_image(image_id);

	uint32_t width = image->get_width();
	uint32_t height = image->get_height();

	uint32_t x = static_cast<uint32_t>(uv.x * static_cast<float>(width));
	uint32_t y = static_cast<uint32_t>(uv.y * static_cast<float>(height));

	// Wrap S
	switch (m_wrapS)
	{
	case EWrapMode::eRepeat:
		x %= width;
		break;
	case EWrapMode::eClampToEdge:
		x = math::clamp(x, 0u, width);
		break;
	case EWrapMode::eMirroredRepeat:
	{
		if ((x / width) % 2u == 0u)
			x = x % width;
		else
			x = width - 1u - (x % width);
	} break;
	default:
		break;
	}

	// Wrap T
	switch (m_wrapT)
	{
	case EWrapMode::eRepeat:
		y %= height;
		break;
	case EWrapMode::eClampToEdge:
		y = math::clamp(y, 0u, height);
		break;
	case EWrapMode::eMirroredRepeat:
	{
		if ((y / height) % 2u == 0u)
			y = y % height;
		else
			y = height - 1u - (y % height);
	} break;
	default:
		break;
	}

	// Sampling
	return nearest_interpolation(image.get(), x, y);
}

math::vec4 CSampler::nearest_interpolation(CImage* image, uint32_t x, uint32_t y)
{
	return unpack_rgba(image->get_pixel(x, y));
}

math::vec4 CSampler::bilinear_interpolation(CImage* image, uint32_t x0, uint32_t y0)
{
	uint32_t width = image->get_width();
	uint32_t heigth = image->get_height();

	uint32_t x1 = (x0 + 1u) % width;
	uint32_t y1 = (y0 + 1u) % heigth;

	x0 = (x0 % width + width) % width;
	y0 = (y0 % heigth + heigth) % heigth;

	math::vec4 Q11 = unpack_rgba(image->get_pixel(x0, y0));
	math::vec4 Q12 = unpack_rgba(image->get_pixel(x0, y1));
	math::vec4 Q21 = unpack_rgba(image->get_pixel(x1, y0));
	math::vec4 Q22 = unpack_rgba(image->get_pixel(x1, y1));

	float x1_x0 = x1 - x1;
	float y1_y0 = y1 - y0;
	float x0_x1 = x0 - x1;
	float y0_y1 = y0 - y1;

	math::vec4 R1 = (x1_x0 * Q11 + x0_x1 * Q21) / x1_x0;
	math::vec4 R2 = (x1_x0 * Q12 + x0_x1 * Q22) / x1_x0;

	math::vec4 P = (y1_y0 * R1 + y0_y1 * R2) / y1_y0;

	return P;
}