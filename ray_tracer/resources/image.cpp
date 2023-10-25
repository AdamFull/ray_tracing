#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <logger/logger.h>

//#include <OpenImageDenoise/oidn.hpp>

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
	m_pData = std::make_unique<glm::vec4[]>(width * height);

	m_uWidth = width;
	m_uHeight = height;
}

void CImage::load(const std::filesystem::path& filepath)
{
	int32_t width{ 0 }, height{ 0 }, components{ 0 };
	uint8_t* image = stbi_load(filepath.string().c_str(), &width, &height, &components, 4);

	create(width, height);

	for (int32_t i = 0; i < width * height; ++i)
		m_pData[i] = unpack_rgba(reinterpret_cast<uint32_t*>(image)[i]);

	stbi_image_free(image);
}

void CImage::load(const uint8_t* data, int32_t size)
{
	int32_t width{ 0 }, height{ 0 }, components{ 0 };
	uint8_t* image = stbi_load_from_memory(data, size, &width, &height, &components, 4);

	create(width, height);

	for (int32_t i = 0; i < width * height; ++i)
		m_pData[i] = unpack_rgba(reinterpret_cast<uint32_t*>(image)[i]);

	stbi_image_free(image);
}

//void CImage::save(const std::filesystem::path& filepath)
//{
//	std::vector<float> colorBuffer(m_uWidth * m_uHeight * 3);
//	for (uint32_t i = 0; i < m_uWidth * m_uHeight; ++i)
//	{
//		colorBuffer[i * 3] = m_pData[i].r;
//		colorBuffer[i * 3 + 1] = m_pData[i].g;
//		colorBuffer[i * 3 + 2] = m_pData[i].b;
//	}
//
//	oidn::DeviceRef device = oidn::newDevice();
//	device.commit();
//
//	std::vector<float> outputBuffer(m_uWidth * m_uHeight * 3);
//
//	oidn::FilterRef filter = device.newFilter("RT");
//	filter.setImage("color", colorBuffer.data(), oidn::Format::Float3, m_uWidth, m_uHeight);
//	filter.setImage("output", outputBuffer.data(), oidn::Format::Float3, m_uWidth, m_uHeight);
//	filter.set("hdr", true); // beauty image is HDR
//	//filter.set("cleanAux", true); // auxiliary images will be prefiltered
//	filter.commit();
//
//	filter.execute();
//
//	const char* errorMessage;
//	if (device.getError(errorMessage) != oidn::Error::None) 
//	{
//		log_error("OIDN error: {}", errorMessage);
//		return;
//	}
//
//	std::unique_ptr<uint32_t[]> temp_data = std::make_unique<uint32_t[]>(m_uWidth * m_uHeight);
//	for (uint32_t i = 0; i < m_uWidth * m_uHeight; ++i)
//	{
//		glm::vec4 color(outputBuffer[i * 3], outputBuffer[i * 3 + 1], outputBuffer[i * 3 + 2], m_pData[i].a);
//		temp_data[i] = pack_rgba(color);
//	}
//
//	stbi_write_png(filepath.string().c_str(), m_uWidth, m_uHeight, 4, temp_data.get(), m_uWidth * sizeof(uint32_t));
//}

void CImage::save(const std::filesystem::path& filepath)
{
	std::unique_ptr<uint32_t[]> temp_data = std::make_unique<uint32_t[]>(m_uWidth * m_uHeight);

	for (uint32_t i = 0; i < m_uWidth * m_uHeight; ++i)
		temp_data[i] = pack_rgba(m_pData[i]);

	stbi_write_png(filepath.string().c_str(), m_uWidth, m_uHeight, 4, temp_data.get(), m_uWidth * sizeof(uint32_t));
}

void CImage::set_pixel(uint32_t x, uint32_t y, const glm::vec4& color)
{
	m_pData[y * m_uWidth + x] = color;
}

const glm::vec4& CImage::get_pixel(uint32_t x, uint32_t y) const
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

glm::vec4 CImage::bilateral_filter(uint32_t x, uint32_t y, uint32_t radius, float sigmaI, float sigmaS)
{
	float weightSum = 0.0f;
	glm::vec4 result = glm::vec4(0.0f);
	const glm::vec4 centerPixel = get_pixel(x, y);

	for (int32_t i = -radius; i <= static_cast<int32_t>(radius); ++i) 
	{
		for (int32_t j = -radius; j <= static_cast<int32_t>(radius); ++j) 
		{
			if (x + i < 0 || x + i >= m_uWidth || y + j < 0 || y + j >= m_uHeight)
				continue;

			glm::vec4 neighborPixel = get_pixel(x + i, y + j);
			float intensityDiff = glm::length(neighborPixel - centerPixel);
			float spatialDiff = std::sqrt(i * i + j * j);

			float weight = std::exp(-intensityDiff * intensityDiff / (2.f * sigmaI * sigmaI) - spatialDiff * spatialDiff / (2.f * sigmaS * sigmaS));
			result += neighborPixel * weight;
			weightSum += weight;
		}
	}
	return result / weightSum;
}