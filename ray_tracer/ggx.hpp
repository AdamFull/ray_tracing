#pragma once


// Normal Distribution function
inline float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0f) + 1.0f;
	return (alpha2) / (std::numbers::pi_v<float> *denom * denom);
}

// Geometric Shadowing function
inline float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
{
	float r = (roughness + 1.0f);
	float k = (r * r) / 8.0f;
	float GL = dotNL / (dotNL * (1.0f - k) + k);
	float GV = dotNV / (dotNV * (1.0f - k) + k);
	return GL * GV;
}

// Fresnel function
inline math::vec3 F_SchlickR(float cosTheta, math::vec3 F0, float roughness)
{
	return F0 + (math::max(math::vec3(1.0f - roughness), F0) - F0) * std::pow(1.0f - cosTheta, 5.0f);
}

// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
inline float random(float x, float y)
{
	float a = 12.9898f;
	float b = 78.233f;
	float c = 43758.5453f;
	float dt = math::dot(math::vec2(x, y), math::vec2(a, b));
	float sn = glm::mod(dt, 3.14f);
	return glm::fract(std::sin(sn) * c);
}

inline math::vec2 hammersley2d(uint32_t i, uint32_t N)
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	uint32_t bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = static_cast<float>(bits) * 2.3283064365386963e-10;
	return math::vec2(static_cast<float>(i) / static_cast<float>(N), rdi);
}

// Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
inline math::vec3 importanceSample_GGX(math::vec2 Xi, float roughness, math::vec3 normal)
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0f * std::numbers::pi_v<float> *Xi.x + random(normal.x, normal.z) * 0.1f;
	float cosTheta = std::sqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
	float sinTheta = std::sqrt(1.0f - cosTheta * cosTheta);
	math::vec3 H = math::vec3(sinTheta * std::cos(phi), sinTheta * std::sin(phi), cosTheta);

	// Tangent space
	math::vec3 up = abs(normal.z) < 0.999 ? math::vec3(0.0f, 0.0f, 1.0f) : math::vec3(1.0f, 0.0f, 0.0f);
	
#if defined(USE_INTRINSICS)
	__m128 tangentX = math::_vec128_normalize(math::_vec128_cross(up.vec128, normal.vec128));
	__m128 tangentY = math::_vec128_normalize(math::_vec128_cross(normal.vec128, tangentX));

	math::vec3 res{};
	
	__m128 resX = _mm_mul_ps(tangentX, _mm_shuffle_ps(H.vec128, H.vec128, _MM_SHUFFLE(0, 0, 0, 0)));
	__m128 resY = _mm_mul_ps(tangentY, _mm_shuffle_ps(H.vec128, H.vec128, _MM_SHUFFLE(1, 1, 1, 1)));
	__m128 resZ = _mm_mul_ps(normal.vec128, _mm_shuffle_ps(H.vec128, H.vec128, _MM_SHUFFLE(2, 2, 2, 2)));

	res.vec128 = math::_vec128_normalize((_mm_add_ps(resX, _mm_add_ps(resY, resZ))));

	return res;
#else
	math::vec3 tangentX = math::normalize(math::cross(up, normal));
	math::vec3 tangentY = math::normalize(math::cross(normal, tangentX));

	// Convert to world Space
	return math::normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
#endif
}

// microfacet model developed by Torrance-Sparrow
inline math::vec3 evaluateCookTorrenceSpecularBRDF(float D, float G, math::vec3 F, float cosThetaI, float cosThetaO) {

	return math::vec3((D * G * F) / (4.f * cosThetaI * cosThetaO));

}

inline math::vec2 compute_brdf(float NoV, float roughness, uint32_t samples)
{
	const math::vec3 N = math::vec3(0.f, 0.f, 1.f);
	math::vec3 V = math::vec3(std::sqrt(1.f - NoV * NoV), 0.f, NoV);

	math::vec2 LUT{ 0.f };

	for (uint32_t idx = 0u; idx < samples; ++idx)
	{
		math::vec2 Xi = hammersley2d(idx, samples);
		math::vec3 H = importanceSample_GGX(Xi, roughness, N);
		math::vec3 L = 2.f * math::dot(V, H) * H - V;

		float dotNL = std::max(math::dot(N, L), 0.0f);
		float dotNV = std::max(math::dot(N, V), 0.0f);
		float dotVH = std::max(math::dot(V, H), 0.0f);
		float dotNH = std::max(math::dot(H, N), 0.0f);

		if (dotNL >= 0.f)
		{
			float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
			float G_Vis = (G * dotVH) / (dotNH * dotNV);
			float Fc = std::pow(1.0f - dotVH, 5.0f);
			LUT += math::vec2((1.0f - Fc) * G_Vis, Fc * G_Vis);
		}
	}

	return LUT / static_cast<float>(samples);
}