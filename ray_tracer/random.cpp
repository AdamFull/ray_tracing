#include "random.h"

thread_local std::mt19937 rnd::gen{ std::random_device()() };
thread_local std::uniform_real_distribution<> rnd::dis{ std::uniform_real_distribution<>(0.f, 1.f) };

float rnd::sample() noexcept
{
	return dis(gen);
}

float rnd::sample(float min, float max) noexcept
{
	return min + (sample() * (max - min));
}

glm::vec2 rnd::sample_vec2() noexcept
{
	return glm::vec2(sample(), sample());
}

glm::vec2 rnd::sample_vec2(float min, float max) noexcept
{
	return glm::vec2(sample(min, max), sample(min, max));
}

glm::vec3 rnd::sample_vec3() noexcept
{
	return glm::vec3(sample(), sample(), sample());
}

glm::vec3 rnd::sample_vec3(float min, float max) noexcept
{
	return glm::vec3(sample(min, max), sample(min, max), sample(min, max));
}

glm::vec3 rnd::sample_cosine_hemisphere(float s, float t) noexcept
{
	float cosTheta = glm::sqrt(glm::max(0.f, 1.f - s));
	float sinTheta = glm::sqrt(s);
	float phi = 2.f * std::numbers::pi_v<float> * t;
	return glm::vec3(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
}

glm::vec3 rnd::sample_ggx_vndf(const glm::vec3& wo, float roughness, float s, float t) noexcept
{
	float alpha = glm::max(0.001f, roughness * roughness);

	glm::vec3 wo_hemi = glm::normalize(glm::vec3(alpha * wo.x, alpha * wo.y, wo.z));

	float length2 = wo_hemi.x * wo_hemi.x + wo_hemi.y * wo_hemi.y;
	glm::vec3 b1 = length2 > 0.f ? glm::vec3(-wo_hemi.y, wo_hemi.x, 0.f) * (1.f / glm::sqrt(length2)) : glm::vec3(1.f, 0.f, 0.f);
	glm::vec3 b2 = glm::cross(wo_hemi, b1);

	float r = glm::sqrt(s);
	float phi = 2.f * std::numbers::pi_v<float> * t;
	float t1 = r * glm::cos(phi);
	float t2 = r * glm::sin(phi);
	float s1 = 0.5f * (1.f + wo_hemi.z);
	t2 = (1.f - s1) * glm::sqrt(1.f - t1 * t1) + s * t2;

	glm::vec3 wh_hemi = t1 * b1 + t2 * b2 + glm::sqrt(glm::max(0.f, 1.f - t1 * t1 - t2 * t2)) * wo_hemi;

	return glm::normalize(glm::vec3(alpha * wh_hemi.x, alpha * wh_hemi.y, glm::max(0.f, wh_hemi.z)));
}

glm::vec3 rnd::sample_hemisphere_ggx(float roughness, glm::vec3 normal) noexcept
{
	float s = sample();
	float t = sample();

	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0f * std::numbers::pi_v<float> * s;
	float cosTheta = math::fsqrt((1.0f - t) / (1.0f + (alpha * alpha - 1.0f) * t));
	float sinTheta = math::fsqrt(1.0f - cosTheta * cosTheta);

	__m128 _normal = _mm_setr_ps(normal.x, normal.y, normal.z, 0.f);
	
	// Tangent space
	__m128 _up = glm::abs(normal.z) < 0.999 ? _mm_setr_ps(0.f, 0.f, 1.f, 0.f) : _mm_setr_ps(1.f, 0.f, 0.f, 0.f);
	
	__m128 _tangentX = math::_vec128_normalize(math::_vec128_cross(_up, _normal));
	__m128 _tangentY = math::_vec128_normalize(math::_vec128_cross(_normal, _tangentX));
	
	__m128 _Hx = _mm_set_ps1(sinTheta * glm::cos(phi));
	__m128 _Hy = _mm_set_ps1(sinTheta * glm::sin(phi));
	__m128 _Hz = _mm_set_ps1(cosTheta);
	
	__m128 _res = math::_vec128_normalize(_mm_add_ps(_mm_mul_ps(_tangentX, _Hx), _mm_add_ps(_mm_mul_ps(_tangentY, _Hy), _mm_mul_ps(_normal, _Hz))));
	
	float _store[4ull];
	_mm_storeu_ps(_store, _res);
	
	return glm::vec3(_store[0ull], _store[1ull], _store[2ull]);
	//return glm::normalize(glm::vec3(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta));
}

//// Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
//inline glm::vec3 importance_sample_ggx(glm::vec2 Xi, float roughness, glm::vec3 normal) noexcept
//{
//	// Maps a 2D point to a hemisphere with spread based on roughness
//	float alpha = roughness * roughness;
//	float phi = 2.0f * std::numbers::pi_v<float> *Xi.x + random(normal.x, normal.z) * 0.1f;
//	float cosTheta = math::fsqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
//	float sinTheta = math::fsqrt(1.0f - cosTheta * cosTheta);
//
//	__m128 _normal = _mm_setr_ps(normal.x, normal.y, normal.z, 0.f);
//
//	// Tangent space
//	__m128 _up = glm::abs(normal.z) < 0.999 ? _mm_setr_ps(0.f, 0.f, 1.f, 0.f) : _mm_setr_ps(1.f, 0.f, 0.f, 0.f);
//
//	__m128 _tangentX = math::_vec128_normalize(math::_vec128_cross(_up, _normal));
//	__m128 _tangentY = math::_vec128_normalize(math::_vec128_cross(_normal, _tangentX));
//
//	__m128 _Hx = _mm_set_ps1(sinTheta * glm::cos(phi));
//	__m128 _Hy = _mm_set_ps1(sinTheta * glm::sin(phi));
//	__m128 _Hz = _mm_set_ps1(cosTheta);
//
//	__m128 _res = math::_vec128_normalize(_mm_add_ps(_mm_mul_ps(_tangentX, _Hx), _mm_add_ps(_mm_mul_ps(_tangentY, _Hy), _mm_mul_ps(_normal, _Hz))));
//
//	float _store[4ull];
//	_mm_storeu_ps(_store, _res);
//
//	return glm::vec3(_store[0ull], _store[1ull], _store[2ull]);
//}