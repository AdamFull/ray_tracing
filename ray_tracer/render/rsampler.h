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

	static glm::vec3 sample_cosine_hemisphere(const glm::vec2& sample) noexcept;
	static glm::vec3 sample_ggx_vndf(const glm::vec3& wo, const glm::vec2& sample, float roughness) noexcept;
private:
	uint32_t m_nsamples_x{};
	uint32_t m_nsamples_y{};
	uint32_t m_samples_per_pixel{};
	uint32_t m_sample_index{ 0u };
	uint32_t m_hashed_pixel_index{ 0u };
	uint32_t m_next_pattern{ 0u };
};