#pragma once

inline glm::vec4 unpack_rgba(uint32_t packed_color)
{
	float a = static_cast<float>((packed_color >> 24u) & 0xFFu) / 255.0f;
	float b = static_cast<float>((packed_color >> 16u) & 0xFFu) / 255.0f;
	float g = static_cast<float>((packed_color >> 8u) & 0xFFu) / 255.0f;
	float r = static_cast<float>(packed_color & 0xFFu) / 255.0f;

	return glm::vec4(r, g, b, a);
}

inline uint32_t pack_rgba(const glm::vec4& color)
{
	uint32_t r = static_cast<uint32_t>(glm::clamp(color.x, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t g = static_cast<uint32_t>(glm::clamp(color.y, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t b = static_cast<uint32_t>(glm::clamp(color.z, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t a = static_cast<uint32_t>(glm::clamp(color.w, 0.f, 1.f) * 255.f) & 0xFFu;

	return (a << 24) | (b << 16) | (g << 8) | r;
}

class CImage
{
public:
	CImage() = default;
	CImage(uint32_t width, uint32_t heigth);
	CImage(const std::filesystem::path& filepath);

	void create(const uint32_t width, const uint32_t height);
	void load(const std::filesystem::path& filepath);
	void load(const uint8_t* data, int32_t size);
	void save(const std::filesystem::path& filepath);

	void set_pixel(uint32_t x, uint32_t y, const glm::vec4& color);
	const glm::vec4& get_pixel(uint32_t x, uint32_t y) const;

	const uint32_t get_width() const;
	const uint32_t get_height() const;

	const std::unique_ptr<glm::vec4[]>& get_raw() const;
private:
	glm::vec4 bilateral_filter(uint32_t x, uint32_t y, uint32_t radius, float sigmaI, float sigmaS);
private:
	uint32_t m_uWidth{ 0u }, m_uHeight{ 0u };
	std::unique_ptr<glm::vec4[]> m_pData{};
};

class CTexture
{
public:
	CTexture() = default;
	CTexture(CResourceManager* resource_manager);
	void create(resource_id_t image_source, resource_id_t sampler_source);

	void initialize_texture_transform_khr(const glm::vec2& offset, const glm::vec2& scale, const float rotation);

	const std::unique_ptr<CImage>& get_image() const;

	glm::vec4 sample(const glm::vec2& uv);
private:
	CResourceManager* m_pResourceManager{ nullptr };

	resource_id_t m_image{ invalid_index }, m_sampler{ invalid_index };

	// KHR_texture_transform
	glm::mat3 m_uv_model{ 1.f };
};