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
inline glm::vec3 importance_sample_ggx(glm::vec2 Xi, float roughness, glm::vec3 normal)
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0f * std::numbers::pi_v<float> *Xi.x + random(normal.x, normal.z) * 0.1f;
	float cosTheta = math::fsqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
	float sinTheta = math::fsqrt(1.0f - cosTheta * cosTheta);

	// SSE version works better!
#if defined(USE_INTRINSICS)
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
#else
	glm::vec3 H = glm::vec3(sinTheta * glm::cos(phi), sinTheta * glm::sin(phi), cosTheta);
	
	// Tangent space
	glm::vec3 up = glm::abs(normal.z) < 0.999 ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(1.0f, 0.0f, 0.0f);
	
	glm::vec3 tangentX = math::normalize(glm::cross(up, normal));
	glm::vec3 tangentY = math::normalize(glm::cross(normal, tangentX));
	
	// Convert to world Space
	return math::normalize(tangentX * H.x + tangentY * H.y + normal * H.z);
#endif
}

// microfacet model developed by Torrance-Sparrow
inline bool calculate_cook_torrance_color_brdf(glm::vec3& out_color, const glm::vec3& L, const glm::vec3& V, const glm::vec3& N, const glm::vec3 diffuse, float roughness, float metallic)
{
	glm::vec3 H = math::normalize(V + L);
	float dotNH = glm::clamp(math::dot(N, H), 0.f, 1.f);
	float dotNV = glm::clamp(math::dot(N, V), 0.f, 1.f);
	float dotNL = glm::clamp(math::dot(N, L), 0.f, 1.f);

	if (dotNL > 0.f)
	{
		// D = Normal distribution (Distribution of the microfacets)
		float D = D_GGX(dotNH, roughness);
		// G = Geometric shadowing term (Microfacets shadowing)
		float G = G_SchlicksmithGGX(dotNL, dotNV, roughness);

		glm::vec3 F0{ 0.04f };
		F0 = glm::mix(F0, diffuse, metallic);

		// F = Fresnel factor (Reflectance depending on angle of incidence)
		glm::vec3 F = F_SchlickR(dotNV, F0, roughness);
		glm::vec3 specular = D * F * G / (4.f * dotNL * dotNV + 0.001f);
		glm::vec3 kD = (glm::vec3(1.0f) - F) * (1.0f - metallic);
		//out_color += (kD * diffuse / std::numbers::pi_v<float> + specular) * dotNL;
		out_color += kD * diffuse + specular;

		return true;
	}

	return false;
}

inline glm::vec3 specularContribution(const glm::vec3& diffuse, const glm::vec3& L, const glm::vec3& V, const glm::vec3& N, const glm::vec3& F0, float metallic, float roughness)
{
#if defined(USE_INTRINSICS) && 0
	// Load variables
	__m128 _L = _mm_setr_ps(L.x, L.y, L.z, 0.f);
	__m128 _V = _mm_setr_ps(V.x, V.y, V.z, 0.f);
	__m128 _N = _mm_setr_ps(N.x, N.y, N.z, 0.f);

	__m128 _H = math::_vec128_normalize(_mm_add_ps(_V, _L));
	float dotNH = glm::clamp(_mm_cvtss_f32(math::_vec128_dot_product(_N, _H)), 0.f, 1.f);
	float dotNV = glm::clamp(_mm_cvtss_f32(math::_vec128_dot_product(_N, _V)), 0.f, 1.f);
	float dotNL = glm::clamp(_mm_cvtss_f32(math::_vec128_dot_product(_N, _L)), 0.f, 1.f);
#else

	glm::vec3 H = math::normalize(V + L);
	float dotNH = glm::clamp(math::dot(N, H), 0.f, 1.f);
	float dotNV = glm::clamp(math::dot(N, V), 0.f, 1.f);
	float dotNL = glm::clamp(math::dot(N, L), 0.f, 1.f);
#endif

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
	glm::vec3 H = math::normalize(V + L);

	float dotNH = glm::clamp(math::dot(N, H), 0.f, 1.f);
	float dotVH = glm::clamp(math::dot(V, H), 0.f, 1.f);

	return D_GGX(dotNH, roughness) * dotNH / (4.0f * dotVH);
}