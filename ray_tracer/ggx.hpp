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

inline __m128 F_SchlickR_sse(const float cosTheta, const __m128& F0, const float roughness) noexcept
{
	return _mm_add_ps(F0, _mm_mul_ps(_mm_sub_ps(_mm_max_ps(_mm_set_ps1(1.f - roughness), F0), F0), _mm_set_ps1(glm::pow(1.0f - cosTheta, 5.0f))));
}

// Based omn http://byteblacksmith.com/improvements-to-the-canonical-one-liner-glsl-rand-for-opengl-es-2-0/
inline float random(float x, float y) noexcept
{
	float a = 12.9898f;
	float b = 78.233f;
	float c = 43758.5453f;
	float dt = glm::dot(glm::vec2(x, y), glm::vec2(a, b));
	float sn = glm::mod(dt, 3.14f);
	return glm::fract(std::sin(sn) * c);
}

// Based on http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_slides.pdf
inline glm::vec3 importance_sample_ggx(glm::vec2 Xi, float roughness, glm::vec3 normal) noexcept
{
	// Maps a 2D point to a hemisphere with spread based on roughness
	float alpha = roughness * roughness;
	float phi = 2.0f * std::numbers::pi_v<float> * Xi.x + random(normal.x, normal.z) * 0.1f;
	float cosTheta = math::fsqrt((1.0f - Xi.y) / (1.0f + (alpha * alpha - 1.0f) * Xi.y));
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
}

inline __m128 mix(const __m128& x, const __m128& y, float a)
{
	__m128 _a = _mm_set_ps1(a);
	return _mm_add_ps(_mm_mul_ps(x, _mm_sub_ps(_mm_set_ps1(1.f), _a)), _mm_mul_ps(y, _a));
}

// microfacet model developed by Torrance-Sparrow
inline bool calculate_cook_torrance_color_brdf(glm::vec3& out_color, const glm::vec3& L, const glm::vec3& V, const glm::vec3& N, const glm::vec3 diffuse, float roughness, float metallic) noexcept
{
#if 0
	__m128 _L = _mm_setr_ps(L.x, L.y, L.z, 0.f);
	__m128 _V = _mm_setr_ps(V.x, V.y, V.z, 0.f);
	__m128 _N = _mm_setr_ps(N.x, N.y, N.z, 0.f);

	__m128 _H = math::_vec128_normalize(_mm_add_ps(_V, _L));
	float dotNH = glm::clamp(_mm_cvtss_f32(math::_vec128_dot_product(_N, _H)), 0.f, 1.f);
	float dotNV = glm::clamp(_mm_cvtss_f32(math::_vec128_dot_product(_N, _V)), 0.f, 1.f);
	float dotNL = glm::clamp(_mm_cvtss_f32(math::_vec128_dot_product(_N, _L)), 0.f, 1.f);

	if (dotNL > 0.f)
	{
		__m128 DG = _mm_set_ps1(D_GGX(dotNH, roughness) * G_SchlicksmithGGX(dotNL, dotNV, roughness));

		__m128 _diffuse = _mm_setr_ps(diffuse.x, diffuse.y, diffuse.z, 0.f);

		__m128 F0 = _mm_set_ps1(0.04f);
		F0 = mix(F0, _diffuse, metallic);

		__m128 F = F_SchlickR_sse(dotNV, F0, roughness);

		__m128 specular = _mm_div_ps(_mm_mul_ps(DG, F), _mm_set_ps1(4.f * dotNL * dotNV + 0.001f));
		__m128 kD = _mm_mul_ps(_mm_sub_ps(_mm_set_ps1(1.f), F), _mm_set_ps1(1.0f - metallic));

		__m128 color = _mm_add_ps(_mm_mul_ps(kD, _diffuse), specular);

		float r[4ull];
		_mm_store_ps(r, color);

		out_color = glm::vec3(r[0ull], r[1ull], r[2ull]);
		return true;
	}
#else
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
#endif

	return false;
}

inline float calculate_pdf(const glm::vec3& L, const glm::vec3& V, const glm::vec3& N, float roughness)
{
	glm::vec3 H = math::normalize(V + L);

	float dotNH = glm::clamp(math::dot(N, H), 0.f, 1.f);
	float dotVH = glm::clamp(math::dot(V, H), 0.f, 1.f);

	return D_GGX(dotNH, roughness) * dotNH / (4.0f * dotVH);
}