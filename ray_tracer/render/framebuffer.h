#pragma once

class CResourceManager;

class CFramebuffer
{
public:
	CFramebuffer() = default;
	CFramebuffer(CResourceManager* resource_manager);

	void create(uint32_t width, uint32_t heigth);

	void set_sample_count(uint32_t sample_count);
	uint32_t get_sample_count() const;

	const math::vec4& get_pixel(uint32_t x, uint32_t y);
	void set_pixel(uint32_t x, uint32_t y, const math::vec4& color);
	void add_pixel(uint32_t x, uint32_t y, const math::vec4& color);

	void clear(const math::vec4& clear_color);
	void present();

	const glm::uvec2& get_extent() const;
	resource_id_t get_image() const;
private:
	uint32_t m_uSamples{ 1u };
	glm::uvec2 m_dimensions{};

	std::unique_ptr<math::vec4[]> m_pColor{};
	resource_id_t m_imageId{ invalid_index };

private:
	CResourceManager* m_pResourceManager{ nullptr };
};