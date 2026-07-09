#pragma once

namespace ImportanceSample
{
	inline glm::vec3 sample_cosine_hemisphere(const glm::vec2& sample) noexcept
	{
		float cosTheta = glm::sqrt(glm::max(0.0f, 1.0f - sample.s));
		float sinTheta = glm::sqrt(sample.s);
		float phi = 2.0f * std::numbers::pi_v<float> *sample.t;
		return glm::vec3(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
	}

	inline glm::vec3 sample_ggx_vndf(const glm::vec3& wo, const glm::vec2& sample, float alpha) noexcept
	{
		// Transform view direction to hemisphere configuration
		glm::vec3 woHemi = glm::normalize(glm::vec3(alpha * wo.x, alpha * wo.y, wo.z));

		// Create orthonormal basis
		float length2 = woHemi.x * woHemi.x + woHemi.y * woHemi.y;
		glm::vec3 b1 = length2 > 0.0f
			? glm::vec3(-woHemi.y, woHemi.x, 0.0f) * (1.0f / std::sqrt(length2))
			: glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 b2 = glm::cross(woHemi, b1);

		// Parameterization of projected area
		float r = std::sqrt(sample.x);
		float phi = 2.0f * std::numbers::pi_v<float> *sample.y;
		float t1 = r * std::cos(phi);
		float t2 = r * std::sin(phi);
		float s = 0.5f * (1.0f + woHemi.z);
		t2 = (1.0f - s) * std::sqrt(1.0f - t1 * t1) + s * t2;

		// Reprojection onto hemisphere
		glm::vec3 whHemi = t1 * b1 + t2 * b2 + std::sqrt(glm::max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * woHemi;

		// Transforming half vector back to ellipsoid configuration
		return glm::normalize(glm::vec3(alpha * whHemi.x, alpha * whHemi.y, glm::max(0.0f, whHemi.z)));
	}

	inline glm::vec3 sample_ggx_vndf_2(const glm::vec3& wo, const glm::vec2& sample, float alpha) noexcept
	{
		// Transform view direction to hemisphere configuration
		glm::vec3 v = glm::normalize(glm::vec3(alpha * wo.x, wo.y, alpha * wo.z));

		// -- Build an orthonormal basis with v, t1, and t2
		//glm::vec3 t1 = (v.y < 0.9999f) ? glm::normalize(glm::cross(v, glm::vec3(0.f, 1.f, 0.f))) : glm::vec3(1.f, 0.f, 0.f);
		//glm::vec3 t2 = glm::cross(t1, v);

		float length2 = v.x * v.x + v.z * v.z;
		glm::vec3 t1 = length2 > 0.0f ? glm::vec3(-v.y, v.z, 0.0f) * (1.0f / std::sqrt(length2)) : glm::vec3(1.0f, 0.0f, 0.0f);
		glm::vec3 t2 = glm::cross(v, t1);

		// -- Choose a point on a disk with each half of the disk weighted proportionally to its projection onto direction v
		float a = 1.0f / (1.0f + v.y);
		float r = std::sqrt(sample.x);
		float phi = (sample.y < a) ? (sample.y / a) * std::numbers::pi_v<float> : std::numbers::pi_v<float> + (sample.y - a) / (1.0f - a) * std::numbers::pi_v<float>;
		float p1 = r * std::cos(phi);
		float p2 = r * std::sin(phi) * ((sample.y < a) ? 1.0f : v.y);

		// -- Calculate the normal in this stretched tangent space
		glm::vec3 n = p1 * t1 + p2 * t2 + static_cast<float>(std::sqrt(std::fmax(0.0f, 1.0f - p1 * p1 - p2 * p2))) * v;

		// -- unstretch and normalize the normal
		return glm::normalize(glm::vec3(alpha * n.x, n.y, alpha * n.z));
	}

	inline glm::vec3 sample_ggx(const glm::vec3& wo, const glm::vec2& sample, float alpha) noexcept
	{
		float phi = 2.f * std::numbers::pi_v<float> *sample.x;
		float cosTheta = std::sqrt((1.f - sample.y) / (1.f + (alpha * alpha - 1.f) * sample.y));
		float sinTheta = std::sqrt(1.f - cosTheta * cosTheta);


		glm::vec3 H{ sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta };

		glm::vec3 up = glm::abs(wo.z) < 0.999f ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(1.f, 0.f, 0.f);
		glm::vec3 tangent = glm::normalize(glm::cross(up, wo));
		glm::vec3 bitangent = glm::cross(wo, tangent);

		return glm::normalize(tangent * H.x + bitangent * H.y + wo * H.z);
	}
}

class CSamplerBase
{
public:
	virtual ~CSamplerBase() = default;

	virtual void begin(uint32_t pixel_index) = 0;
	virtual void next() = 0;

	virtual float sample() = 0;
	virtual glm::vec2 sample_vec2() = 0;
protected:
	uint32_t m_samples_per_pixel{};
};

class CCMJSampler : public CSamplerBase
{
public:
	CCMJSampler(uint32_t samples_per_pixel);
	~CCMJSampler() = default;

	void begin(uint32_t pixel_index) override;
	void next() override;

	float sample() override;
	glm::vec2 sample_vec2() override;
private:
	uint32_t m_nsamples_x{};
	uint32_t m_nsamples_y{};
	uint32_t m_sample_index{ 0u };
	uint32_t m_hashed_pixel_index{ 0u };
	uint32_t m_next_pattern{ 0u };
};


class CPCGSampler : public CSamplerBase
{
public:
    CPCGSampler(uint32_t samples_per_pixel);
    ~CPCGSampler() = default;

    void begin(uint32_t pixel_index) override;
    void next() override;

    float sample() override;
    glm::vec2 sample_vec2() override;

	// Re-seed the stream. Used by the interactive preview to decorrelate samples per
	// pixel and per accumulated frame so the image refines progressively.
	void reseed(uint64_t initstate, uint64_t initseq) { seed(initstate, initseq); }

	static constexpr uint64_t defaultState = 1753877967969059832ull;
	static constexpr uint64_t defaultInc = 109ull;
protected:
	void seed(uint64_t initstate, uint64_t initseq);
	uint32_t uniformUint32();

private:
	uint64_t state_{ defaultState };
	uint64_t inc_{ defaultInc };
};


class CCRandomSampler : public CSamplerBase
{
public:
	CCRandomSampler(uint32_t samples_per_pixel);
	~CCRandomSampler() = default;

	void begin(uint32_t pixel_index) override;
	void next() override;

	float sample() override;
	glm::vec2 sample_vec2() override;
};