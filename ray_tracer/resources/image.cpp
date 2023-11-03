#include "resource_manager.h"

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

const uint32_t CImage::get_width() const
{
	return m_uWidth;
}

const uint32_t CImage::get_height() const
{
	return m_uHeight;
}

const std::unique_ptr<glm::vec4[]>& CImage::get_raw() const
{
	return m_pData;
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



CTexture::CTexture(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CTexture::create(resource_id_t image_source, resource_id_t sampler_source)
{
	m_image = image_source;
	m_sampler = sampler_source;
}

void CTexture::initialize_texture_transform_khr(const glm::vec2& offset, const glm::vec2& scale, const float rotation)
{
	glm::mat3 translation_m = glm::mat3(1.f, 0.f, 0.f, 0.f, 1.f, 0.f, offset.x, offset.y, 1.f);

	float sin_theta = glm::sin(rotation);
	float cos_theta = glm::cos(rotation);
	glm::mat3 rotation_m = glm::mat3(cos_theta, sin_theta, 0.f, -sin_theta, cos_theta, 0.f, 0.f, 0.f, 1.f);
	glm::mat3 scale_m = glm::mat3(scale.x, 0.f, 0.f, 0.f, scale.y, 0.f, 0.f, 0.f, 1.f);

	m_uv_model = translation_m * rotation_m * scale_m;
}

const std::unique_ptr<CImage>& CTexture::get_image() const
{
	return m_pResourceManager->get_image(m_image);
}

glm::vec4 CTexture::sample(const glm::vec2& uv)
{
	auto& sampler = m_pResourceManager->get_sampler(m_sampler);
	auto& image = m_pResourceManager->get_image(m_image);

	auto n_uv = glm::vec2(m_uv_model * glm::vec3(uv, 1.f));

	float img_width = static_cast<float>(image->get_width());
	float img_height = static_cast<float>(image->get_height());

	glm::vec2 texcoord = sampler->wrap(n_uv);
	return sampler->interpolate(image->get_raw().get(), texcoord, img_width, img_height);
}