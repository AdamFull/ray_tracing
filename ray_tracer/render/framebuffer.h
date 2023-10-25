#pragma once

enum EFBAttachmentFlags
{
	FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT = 1 << 0,
	FRAMEBUFFER_ALBEDO_ATTACHMENT_FLAG_BIT = 1 << 1,
	FRAMEBUFFER_NORMAL_ATTACHMENT_FLAG_BIT = 1 << 2,
	FRAMEBUFFER_ALL_ATTACHMENTS_FLAG_BIT = FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT | FRAMEBUFFER_ALBEDO_ATTACHMENT_FLAG_BIT | FRAMEBUFFER_NORMAL_ATTACHMENT_FLAG_BIT
};

class CResourceManager;

class CFramebuffer
{
public:
	CFramebuffer() = default;
	CFramebuffer(CResourceManager* resource_manager);

	void create(uint32_t width, uint32_t heigth, uint32_t attachment_mask = FRAMEBUFFER_ALL_ATTACHMENTS_FLAG_BIT);

	const glm::vec3& get_pixel(uint32_t x, uint32_t y, uint32_t attachment_mask);
	void set_pixel(uint32_t x, uint32_t y, const glm::vec3& color, uint32_t attachment_mask);
	void add_pixel(uint32_t x, uint32_t y, const glm::vec3& color, uint32_t attachment_mask);

	void clear(const glm::vec3& clear_color, uint32_t attachment_mask);
	void present();

	const glm::uvec2& get_extent() const;
	resource_id_t get_image() const;
private:
	glm::uvec2 m_dimensions{};

	std::unordered_map<uint32_t, std::unique_ptr<glm::vec3[]>> m_colorBuffers;

	resource_id_t m_imageId{ invalid_index };

private:
	CResourceManager* m_pResourceManager{ nullptr };
	float m_gamma{ 2.2f };
	float m_exposure{ 4.f };
};
