#pragma once

inline uint32_t pack_color_u32(const math::vec4& color)
{
	uint32_t r = static_cast<uint32_t>(math::clamp(color.x, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t g = static_cast<uint32_t>(math::clamp(color.y, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t b = static_cast<uint32_t>(math::clamp(color.z, 0.f, 1.f) * 255.f) & 0xFFu;
	uint32_t a = static_cast<uint32_t>(math::clamp(color.w, 0.f, 1.f) * 255.f) & 0xFFu;

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
	void save(const std::filesystem::path& filepath);

	void set_pixel(uint32_t x, uint32_t y, uint32_t color);
	uint32_t get_pixel(uint32_t x, uint32_t y) const;

	void set_sampler(resource_id_t id);
	resource_id_t get_sampler() const;

	const uint32_t get_width() const;
	const uint32_t get_height() const;
private:
	uint32_t m_uWidth{ 0u }, m_uHeight{ 0u };
	std::unique_ptr<uint32_t[]> m_pData{};
	resource_id_t m_sampler{ invalid_index };
};