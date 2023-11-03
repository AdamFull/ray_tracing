#pragma once

inline float cos_theta(const glm::vec3& w) noexcept { return w.z; }
inline float cos_theta_sq(const glm::vec3& w) noexcept { return w.z * w.z; }
inline float sin_theta_sq(const glm::vec3& w) noexcept { return glm::max(0.f, 1.f - cos_theta_sq(w)); }
inline float sin_theta(const glm::vec3& w) noexcept { return glm::sqrt(sin_theta_sq(w)); }
inline float tan_theta(const glm::vec3& w) noexcept { return sin_theta(w) / cos_theta(w); }
inline float tan_theta_sq(const glm::vec3& w) noexcept { return sin_theta_sq(w) / cos_theta_sq(w); }
inline bool on_same_hemisphere(const glm::vec3& wi, const glm::vec3& wo) noexcept { return wi.z * wo.z > 0.f; }

// ggx
inline float D_GGX(const glm::vec3& wh, float alpha) noexcept
{
	float alphaSq = alpha * alpha;
	float a = 1.0f + cos_theta_sq(wh) * (alphaSq - 1.0f);
	return alphaSq / (std::numbers::pi_v<float> * a * a);
}

inline float Smith_G1_GGX(const glm::vec3& w, float alpha) noexcept
{
	float tanTheraSq = tan_theta_sq(w);
	if (std::isinf(tanTheraSq)) return 0.0f;
	float alphaSq = alpha * alpha;
	assert(alphaSq * tanTheraSq >= -1.0f);
	float lambda = (-1.0f + std::sqrt(alphaSq * tanTheraSq + 1.0f)) / 2.0f;
	return 1.0f / (1.0f + lambda);
}

inline float G2_Smith_Uncorrelated_GGX(const glm::vec3& wi, const glm::vec3& wo, float alpha)
{
	return Smith_G1_GGX(wi, alpha) * Smith_G1_GGX(wo, alpha);
}

inline float Smith_G2_Height_Correlated_GGX(const glm::vec3& wi, const glm::vec3& wo, float alpha)
{
	float tan2ThetaO = tan_theta_sq(wo);
	float tan2ThetaI = tan_theta_sq(wi);
	if (std::isinf(tan2ThetaO)) return 0.0f;
	if (std::isinf(tan2ThetaI)) return 0.0f;
	float alpha2 = alpha * alpha;
	assert(alpha2 * tan2ThetaO >= -1.0f);
	assert(alpha2 * tan2ThetaI >= -1.0f);
	float lambda_wo = (-1.0f + std::sqrt(alpha2 * tan2ThetaO + 1.0f)) / 2.0f;
	float lambda_wi = (-1.0f + std::sqrt(alpha2 * tan2ThetaI + 1.0f)) / 2.0f;
	return 1.0f / (1.0f + lambda_wo + lambda_wi);
}

inline glm::vec3 F0_Schlick(float eta) noexcept
{
	float a = (1.f - eta) / (1.f + eta);
	return glm::vec3(a * a);
}

inline glm::vec3 F_Schlick(float cosThetaI, const glm::vec3& f0) noexcept
{
	float a = glm::max(0.0f, 1.0f - cosThetaI);
	float a2 = a * a;
	float a5 = a2 * a2 * a;
	return f0 + (1.0f - f0) * a5;
}

inline glm::vec3 lambert_diffuse(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& diffuse_color)
{
	if (!on_same_hemisphere(wi, wo))
		return glm::vec3(0.f);

	return diffuse_color / std::numbers::pi_v<float>;
}

inline glm::vec3 microfacet_reflection_ggx(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& n, const glm::vec3& f0, float eta, float alpha)
{
	if (!on_same_hemisphere(wi, wo) || cos_theta(wi) == 0.0f || cos_theta(wo) == 0.0f)
		return glm::vec3(0.0f);
	
	glm::vec3 wh = wi + wo;
	if (wh.x == 0.0f && wh.y == 0.0f && wh.z == 0.0f)
		return glm::vec3(0.0f);
	wh = glm::normalize(wh);
	
	glm::vec3 F{};
	if (eta < 1.0f) 
	{
		float cosThetaT = glm::dot(wi, wh);
		float cos2ThetaT = cosThetaT * cosThetaT;
		F = cos2ThetaT > 0.0f ? F_Schlick(glm::abs(cosThetaT), f0) : glm::vec3(1.0f);
	}
	else
		F = F_Schlick(glm::abs(glm::dot(wh, wo)), f0);
	
	float G = Smith_G2_Height_Correlated_GGX(wi, wo, alpha);
	float D = D_GGX(wh, alpha);

	return F * G * D / (4.0f * glm::abs(cos_theta(wi)) * glm::abs(cos_theta(wo)));
}

inline glm::vec3 microfacet_transmission_ggx(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& f0, float eta, float alpha)
{
	if (on_same_hemisphere(wi, wo) || cos_theta(wi) == 0.0f || cos_theta(wo) == 0.0f)
		return glm::vec3(0.0f);

	glm::vec3 wh = glm::normalize(wi + eta * wo);
	if (cos_theta(wh) < 0.0f)
		wh = -wh;

	bool sameSide = glm::dot(wo, wh) * glm::dot(wi, wh) > 0.0f;
	if (sameSide)
		return glm::vec3(0.0f);

	glm::vec3 F{};
	if (eta < 1.0f) 
	{
		float cosThetaT = glm::dot(wi, wh);
		float cos2ThetaT = cosThetaT * cosThetaT;
		F = cos2ThetaT > 0.0f ? F_Schlick(glm::abs(cosThetaT), f0) : glm::vec3(1.0f);
	}
	else
		F = F_Schlick(abs(dot(wh, wo)), f0);

	float G = Smith_G2_Height_Correlated_GGX(wi, wo, alpha);
	float D = D_GGX(wh, alpha);
	float denomSqrt = glm::dot(wi, wh) + eta * glm::dot(wo, wh);
	return (glm::vec3(1.0f) - F) * D * G * glm::abs(glm::dot(wi, wh)) * glm::abs(glm::dot(wo, wh)) / (denomSqrt * denomSqrt * glm::abs(cos_theta(wi)) * abs(cos_theta(wo)));
}

inline float cosine_weighted_pdf(const glm::vec3& wi, const glm::vec3& wo) noexcept
{
	return on_same_hemisphere(wi, wo) ? cos_theta(wi) / std::numbers::pi_v<float> : 0.f;
}

inline float ggx_vndf_reflection_pdf(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& n, float alpha) noexcept
{
	if (!on_same_hemisphere(wi, wo))
		return 0.0f;
	
	glm::vec3 wh = glm::normalize(wi + wo);
	float pdf_h = Smith_G1_GGX(wo, alpha) * D_GGX(wh, alpha) * glm::abs(glm::dot(wh, wo)) / glm::abs(cos_theta(wo));
	float dwh_dwi = 1.0f / (4.0f * glm::dot(wi, wh));
	return pdf_h * dwh_dwi;
}

inline float ggx_vndf_transmission_pdf(const glm::vec3& wi, const glm::vec3& wo, float eta, float alpha) noexcept
{
	if (on_same_hemisphere(wi, wo))
		return 0.0f;
	
	glm::vec3 wh = glm::normalize(wi + eta * wo);
	bool sameSide = glm::dot(wo, wh) * glm::dot(wi, wh) > 0.0f;
	if (sameSide) return 0.0f;
	
	float pdf_h = Smith_G1_GGX(wo, alpha) * D_GGX(wh, alpha) * glm::abs(glm::dot(wh, wo)) / glm::abs(cos_theta(wo));
	float sqrtDenom = glm::dot(wi, wh) + eta * glm::dot(wo, wh);
	float dwh_dwi = glm::abs(glm::dot(wi, wh)) / (sqrtDenom * sqrtDenom);
	return pdf_h * dwh_dwi;
}

inline float ggx_reflection_pdf(const glm::vec3& wi, const glm::vec3& wo, float alpha) noexcept
{
	if (!on_same_hemisphere(wi, wo))
		return 0.0f;

	glm::vec3 wh = glm::normalize(wi + wo);
	return D_GGX(wh, alpha) * cos_theta(wh) / (4.f * glm::dot(wo, wh));
}

inline float ggx_transmission_pdf(const glm::vec3& wi, const glm::vec3& wo, float eta, float alpha) noexcept
{
	if (on_same_hemisphere(wi, wo))
		return 0.0f;

	glm::vec3 wh = glm::normalize(wi + wo);
	return D_GGX(wh, alpha) * cos_theta(wh) / (4.f * glm::dot(wo, wh));
}