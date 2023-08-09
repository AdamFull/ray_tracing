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
inline glm::vec3 F_Schlick(float cosTheta, float f90, glm::vec3 F0)
{
	return F0 + (f90 - F0) * glm::pow(1.0f - cosTheta, 5.0f);
}

inline glm::vec3 F_SchlickR(float cosTheta, glm::vec3 F0, float roughness)
{
	return F0 + (glm::max(glm::vec3(1.0f - roughness), F0) - F0) * glm::pow(1.0f - cosTheta, 5.0f);
}

// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
inline float random(float x, float y)
{
	float a = 12.9898f;
	float b = 78.233f;
	float c = 43758.5453f;
	float dt = glm::dot(glm::vec2(x, y), glm::vec2(a, b));
	float sn = glm::mod(dt, 3.14f);
	return glm::fract(std::sin(sn) * c);
}

inline glm::vec2 hammersley2d(uint32_t i, uint32_t N)
{
	// Radical inverse based on http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
	uint32_t bits = (i << 16u) | (i >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	float rdi = static_cast<float>(bits) * 2.3283064365386963e-10;
	return glm::vec2(static_cast<float>(i) / static_cast<float>(N), rdi);
}

// Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
inline glm::vec3 importanceSample_GGX(glm::vec2 Xi, float roughness, glm::vec3 normal)
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0f * std::numbers::pi_v<float> *Xi.x + random(normal.x, normal.z) * 0.1f;
	float cosTheta = glm::sqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
	float sinTheta = glm::sqrt(1.0f - cosTheta * cosTheta);
	glm::vec3 H = glm::vec3(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);

	// Tangent space
	glm::vec3 up = abs(normal.z) < 0.999 ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
	
#if defined(USE_INTRINSICS) && defined(USE_INTRINSICS_GGX)
	__m128 tangentX = glm::_vec128_normalize(glm::_vec128_cross(up.vec128, normal.vec128));
	__m128 tangentY = glm::_vec128_normalize(glm::_vec128_cross(normal.vec128, tangentX));

	glm::vec3 res{};
	
	__m128 resX = _mm_mul_ps(tangentX, _mm_shuffle_ps(H.vec128, H.vec128, _MM_SHUFFLE(0, 0, 0, 0)));
	__m128 resY = _mm_mul_ps(tangentY, _mm_shuffle_ps(H.vec128, H.vec128, _MM_SHUFFLE(1, 1, 1, 1)));
	__m128 resZ = _mm_mul_ps(normal.vec128, _mm_shuffle_ps(H.vec128, H.vec128, _MM_SHUFFLE(2, 2, 2, 2)));

	res.vec128 = glm::_vec128_normalize((_mm_add_ps(resX, _mm_add_ps(resY, resZ))));

	return res;
#else
	glm::vec3 tangentX = glm::normalize(glm::cross(up, normal));
	glm::vec3 tangentY = glm::normalize(glm::cross(normal, tangentX));

	// Convert to world Space
	return glm::normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
#endif
}

// microfacet model developed by Torrance-Sparrow
inline glm::vec3 evaluateCookTorrenceSpecularBRDF(float D, float G, glm::vec3 F, float cosThetaI, float cosThetaO) {

	return glm::vec3((D * G * F) / (4.f * cosThetaI * cosThetaO));

}

inline glm::vec3 specularContribution(const glm::vec3& diffuse, const glm::vec3& L, const glm::vec3& V, const glm::vec3& N, const glm::vec3& F0, float metallic, float roughness)
{
	glm::vec3 H = glm::normalize(V + L);
	float dotNH = glm::clamp(glm::dot(N, H), 0.f, 1.f);
	float dotNV = glm::clamp(glm::dot(N, V), 0.f, 1.f);
	float dotNL = glm::clamp(glm::dot(N, L), 0.f, 1.f);

	glm::vec3 out_color{ 0.f };

	if (dotNL > 0.f)
	{
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
		// F = Fresnel factor (Reflectance depending on angle of incidence)
		glm::vec3 F = F_Schlick(dotNV, 1.f, F0);
		glm::vec3 spec = D * F * G / (4.f * dotNL * dotNV + 0.001f);
		glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);
		out_color += (kD * diffuse / std::numbers::pi_v<float> + spec) * dotNL;
	}

	return out_color;
}

inline float calculate_pdf(const glm::vec3& L, const glm::vec3& V, const glm::vec3& N, float roughness)
{
	glm::vec3 H = glm::normalize(V + L);

	float dotNH = glm::clamp(glm::dot(N, H), 0.f, 1.f);
	float dotVH = glm::clamp(glm::dot(V, H), 0.f, 1.f);

	return D_GGX(dotNH, roughness) * dotNH / (4.0f * dotVH);
}

inline glm::vec2 compute_brdf(float NoV, float roughness, uint32_t samples)
{
	const glm::vec3 N = glm::vec3(0.f, 0.f, 1.f);
	glm::vec3 V = glm::vec3(glm::sqrt(1.f - NoV * NoV), 0.f, NoV);

	glm::vec2 LUT{ 0.f };

	for (uint32_t idx = 0u; idx < samples; ++idx)
	{
		glm::vec2 Xi = hammersley2d(idx, samples);
		glm::vec3 H = importanceSample_GGX(Xi, roughness, N);
		glm::vec3 L = 2.f * glm::dot(V, H) * H - V;

		float dotNL = glm::max(glm::dot(N, L), 0.0f);
		float dotNV = glm::max(glm::dot(N, V), 0.0f);
		float dotVH = glm::max(glm::dot(V, H), 0.0f);
		float dotNH = glm::max(glm::dot(H, N), 0.0f);

		if (dotNL >= 0.f)
		{
			float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);
			float G_Vis = (G * dotVH) / (dotNH * dotNV);
			float Fc = glm::pow(1.0f - dotVH, 5.0f);
			LUT += glm::vec2((1.0f - Fc) * G_Vis, Fc * G_Vis);
		}
	}

	return LUT / static_cast<float>(samples);
}