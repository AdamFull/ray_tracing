#include "rsampler.h"

unsigned long count_lead_zeros(unsigned long x)
{
	assert(x != 0ul);
#ifdef _MSC_VER
	unsigned long index{ 0ul };
	_BitScanReverse(&index, x);
	return 31ul - index;
#else
	return __builtin_clz(x);
#endif
}

uint32_t cmj_permute(uint32_t i, uint32_t l, uint32_t p)
{
	assert(l > 0);
	uint32_t w = (1 << (32 - count_lead_zeros(l - 1))) - 1;

	do {
		i ^= p;             i *= 0xe170893d;
		i ^= p >> 16;
		i ^= (i & w) >> 4;
		i ^= p >> 8;        i *= 0x0929eb3f;
		i ^= p >> 23;
		i ^= (i & w) >> 1;  i *= 1 | p >> 27;
		i *= 0x6935fa69;
		i ^= (i & w) >> 11; i *= 0x74dcb303;
		i ^= (i & w) >> 2;  i *= 0x9e501cc3;
		i ^= (i & w) >> 2;  i *= 0xc860a3df;
		i &= w;
		i ^= i >> 5;
	} while (i >= l);

	return (i + p) % l;
}

float cmj_rnd_rloat(uint32_t i, uint32_t p)
{
	i ^= p;
	i ^= i >> 17;
	i ^= i >> 10;       i *= 0xb36534e5;
	i ^= i >> 12;
	i ^= i >> 21;       i *= 0x93fc4795;
	i ^= 0xdf6e307f;
	i ^= i >> 17;       i *= 1 | p >> 18;
	return i * (1.f / 4294967808.0f);
}

inline uint64_t hash(uint64_t k) 
{
	k ^= k >> 33;
	k *= 0xff51afd7ed558ccdllu;
	k ^= k >> 33;
	k *= 0xc4ceb9fe1a85ec53llu;
	k ^= k >> 33;
	return k;
}

CCMGSampler::CCMGSampler(uint32_t samples_per_pixel)
{
	m_nsamples_x = static_cast<uint32_t>(glm::sqrt(samples_per_pixel));
	m_nsamples_y = (samples_per_pixel - 1u) / m_nsamples_x + 1u;
	m_samples_per_pixel = m_nsamples_x * m_nsamples_y;
}

void CCMGSampler::begin(uint32_t pixel_index)
{
	m_hashed_pixel_index = hash(pixel_index);
	m_sample_index = 0u;
	m_next_pattern = 0u;
}

void CCMGSampler::next()
{
	m_sample_index++;
	m_next_pattern = 0u;
}

float CCMGSampler::sample() noexcept
{
	uint32_t n = m_samples_per_pixel;
	uint32_t p = m_hashed_pixel_index + m_next_pattern;
	m_next_pattern++;

	uint32_t s = m_sample_index;
	uint32_t sx = cmj_permute(s, n, p * 0x68bc21eb);
	float jx = cmj_rnd_rloat(s, p * 0x967a889b);
	float x = (sx + jx) / n;

	return glm::min(x, 1.f - std::numeric_limits<float>::epsilon());
}

float CCMGSampler::sample(float min, float max) noexcept
{
	return min + (sample() * (max - min));
}

glm::vec2 CCMGSampler::sample_vec2() noexcept
{
	uint32_t m = m_nsamples_x;
	uint32_t n = m_nsamples_y;
	uint32_t p = m_hashed_pixel_index + m_next_pattern;
	m_next_pattern++;

	uint32_t s = cmj_permute(m_sample_index, m_samples_per_pixel, p * 0x51633e2d);
	uint32_t sx = cmj_permute(s % m, m, p * 0x68bc21eb);
	uint32_t sy = cmj_permute(s / m, n, p * 0x02e5be93);
	float jx = cmj_rnd_rloat(s, p * 0x967a889b);
	float jy = cmj_rnd_rloat(s, p * 0x368cc8b7);
	float x = (sx + (sy + jx) / n) / m;
	float y = (s + jy) / m_samples_per_pixel;

	x = glm::min(x, 1.f - std::numeric_limits<float>::epsilon());
	y = glm::min(y, 1.f - std::numeric_limits<float>::epsilon());

	return glm::vec2(x, y);
}

glm::vec3 CCMGSampler::sample_cosine_hemisphere(const glm::vec2& sample) noexcept
{
	float cosTheta = glm::sqrt(glm::max(0.0f, 1.0f - sample.s));
	float sinTheta = glm::sqrt(sample.s);
	float phi = 2.0f * std::numbers::pi_v<float> * sample.t;
	return glm::vec3(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
}

glm::vec3 CCMGSampler::sample_ggx_vndf(const glm::vec3& wo, const glm::vec2& sample, float alpha) noexcept
{
	// Transform view direction to hemisphere configuration
	glm::vec3 woHemi = glm::normalize(glm::vec3(alpha * wo.x, alpha * wo.y, wo.z));

	// Create orthonormal basis
	float length2 = woHemi.x * woHemi.x + woHemi.y * woHemi.y;
	glm::vec3 b1 = length2 > 0.0f
		? glm::vec3(-woHemi.y, woHemi.x, 0.0f) * (1.0f / glm::sqrt(length2))
		: glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 b2 = glm::cross(woHemi, b1);

	// Parameterization of projected area
	float r = glm::sqrt(sample.s);
	float phi = 2.0f * std::numbers::pi_v<float> * sample.t;
	float t1 = r * glm::cos(phi);
	float t2 = r * glm::sin(phi);
	float s = 0.5f * (1.0f + woHemi.z);
	t2 = (1.0f - s) * glm::sqrt(1.0f - t1 * t1) + s * t2;

	// Reprojection onto hemisphere
	glm::vec3 whHemi = t1 * b1 + t2 * b2 + glm::sqrt(glm::max(0.0f, 1.0f - t1 * t1 - t2 * t2)) * woHemi;

	// Transforming half vector back to ellipsoid configuration
	return glm::normalize(glm::vec3(alpha * whHemi.x, alpha * whHemi.y, glm::max(0.0f, whHemi.z)));
}