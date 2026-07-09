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
	return i * (1.0f / 4294967808.0f);
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

namespace ImportanceSample
{
	
}

CCMJSampler::CCMJSampler(uint32_t samples_per_pixel) 
{
	m_nsamples_x = static_cast<uint32_t>(glm::sqrt(samples_per_pixel));
	m_nsamples_y = (samples_per_pixel - 1u) / m_nsamples_x + 1u;
	m_samples_per_pixel = m_nsamples_x * m_nsamples_y;
}

void CCMJSampler::begin(uint32_t pixel_index)
{
	m_hashed_pixel_index = hash(pixel_index);
	m_sample_index = 0u;
	m_next_pattern = 0u;
}

void CCMJSampler::next()
{
	m_sample_index++;
	m_next_pattern = 0u;
}

float CCMJSampler::sample()
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

//float CCMJSampler::sample(float min, float max) noexcept
//{
//	return min + (sample() * (max - min));
//}

glm::vec2 CCMJSampler::sample_vec2()
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

//
//x = static_cast<float>(((double)rand() / (RAND_MAX)) + 1.f);
//y = static_cast<float>(((double)rand() / (RAND_MAX)) + 1.f);

CPCGSampler::CPCGSampler(uint32_t samples_per_pixel)
{
	m_samples_per_pixel = samples_per_pixel;
}

void CPCGSampler::begin(uint32_t pixel_index)
{
	
}

void CPCGSampler::next()
{
}

float CPCGSampler::sample()
{
	float x = static_cast<float>(uniformUint32()) / 0xffffffff;
	x = glm::min(x, 1.f - std::numeric_limits<float>::epsilon());
	return x;
}

glm::vec2 CPCGSampler::sample_vec2()
{
	float x = static_cast<float>(uniformUint32()) / 0xffffffff;
	x = glm::min(x, 1.f - std::numeric_limits<float>::epsilon());

	float y = static_cast<float>(uniformUint32()) / 0xffffffff;
	y = glm::min(y, 1.f - std::numeric_limits<float>::epsilon());
	return glm::vec2(x, y);
}

void CPCGSampler::seed(uint64_t initstate, uint64_t initseq)
{
	state_ = 0U;
	inc_ = (initseq << 1u) | 1u;
	uniformUint32();
	state_ += initstate;
	uniformUint32();
}

uint32_t CPCGSampler::uniformUint32()
{
	uint64_t oldState = state_;
	state_ = oldState * 6364136223846793005ull + (inc_ | 1);
	uint32_t xorShifted = static_cast<uint32_t>(((oldState >> 18u) ^ oldState) >> 27u);
	uint32_t rot = static_cast<uint32_t>(oldState >> 59u);
	return (xorShifted >> rot) | (xorShifted << ((~rot + 1u) & 31));
}



CCRandomSampler::CCRandomSampler(uint32_t samples_per_pixel)
{

}

void CCRandomSampler::begin(uint32_t pixel_index)
{
}

void CCRandomSampler::next()
{
}

float CCRandomSampler::sample()
{
	float x = static_cast<float>((double)rand() / (RAND_MAX));
	x = glm::min(x, 1.f - std::numeric_limits<float>::epsilon());
	return x;
}

glm::vec2 CCRandomSampler::sample_vec2()
{
	float x = static_cast<float>((double)rand() / (RAND_MAX));
	x = glm::min(x, 1.f - std::numeric_limits<float>::epsilon());

	float y = static_cast<float>((double)rand() / (RAND_MAX));
	y = glm::min(y, 1.f - std::numeric_limits<float>::epsilon());

	return glm::vec2(x, y);
}