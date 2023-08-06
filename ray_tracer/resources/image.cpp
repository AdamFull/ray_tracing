#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

CImage::CImage(uint32_t width, uint32_t heigth)
{
	create(width, heigth);
}

CImage::CImage(const std::filesystem::path& filepath)
{
	load(filepath);
}

void CImage::create(const uint32_t width, const uint32_t height)
{
	m_pData = std::make_unique<uint32_t[]>(width * height);

	m_uWidth = width;
	m_uHeight = height;
}

void CImage::load(const std::filesystem::path& filepath)
{
	int32_t width{ 0 }, height{ 0 }, components{ 0 };
	uint8_t* image = stbi_load(filepath.string().c_str(), &width, &height, &components, 4);

	create(width, height);

	std::memmove(m_pData.get(), image, m_uWidth * m_uHeight * 4u);

	stbi_image_free(image);
}

void CImage::save(const std::filesystem::path& filepath)
{
	stbi_write_png(filepath.string().c_str(), m_uWidth, m_uHeight, 4, m_pData.get(), m_uWidth * sizeof(uint32_t));
}

void CImage::set_pixel(uint32_t x, uint32_t y, uint32_t color)
{
	m_pData[y * m_uWidth + x] = color;
}

uint32_t CImage::get_pixel(uint32_t x, uint32_t y) const
{
	return m_pData[y * m_uWidth + x];
}

void CImage::set_sampler(resource_id_t id)
{
	m_sampler = id;
}

resource_id_t CImage::get_sampler() const
{
	return m_sampler;
}

const uint32_t CImage::get_width() const
{
	return m_uWidth;
}

const uint32_t CImage::get_height() const
{
	return m_uHeight;
}