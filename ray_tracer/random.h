#pragma once

#include <random>

struct rnd
{
	static float sample() noexcept;
	static float sample(float min, float max) noexcept;
	static glm::vec2 sample_vec2() noexcept;
	static glm::vec2 sample_vec2(float min, float max) noexcept;
	static glm::vec3 sample_vec3() noexcept;
	static glm::vec3 sample_vec3(float min, float max) noexcept;

	static glm::vec3 sample_cosine_hemisphere(float s, float t) noexcept;
	static glm::vec3 sample_ggx_vndf(const glm::vec3& wo, float roughness, float s, float t) noexcept;
	static glm::vec3 sample_hemisphere_ggx(float roughness, glm::vec3 normal) noexcept;
private:
	thread_local static std::mt19937 gen;
	thread_local static std::uniform_real_distribution<> dis;
};