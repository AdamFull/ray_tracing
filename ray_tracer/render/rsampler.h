#pragma once

class CCMGSampler
{
public:
	CCMGSampler(uint32_t samples_per_pixel);

	void begin(uint32_t pixel_index);
	void next();

	float sample() noexcept;
	float sample(float min, float max) noexcept;
	glm::vec2 sample_vec2() noexcept;

	static glm::vec3 sample_cosine_hemisphere(float s, float t) noexcept;
	static glm::vec3 sample_ggx_vndf(const glm::vec3& wo, float roughness, float s, float t) noexcept;
private:
	uint32_t m_nsamples_x{};
	uint32_t m_nsamples_y{};
	uint32_t m_samples_per_pixel{};
	uint32_t m_sample_index{ 0u };
	uint32_t m_hashed_pixel_index{ 0u };
	uint32_t m_next_pattern{ 0u };
};