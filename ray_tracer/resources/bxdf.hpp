#pragma once

inline float cos_theta(const glm::vec3& w) noexcept
{
	return w.z;
}

inline float cos_theta_sq(const glm::vec3& w) noexcept
{
	return cos_theta(w) * cos_theta(w);
}

inline float sin_theta_sq(const glm::vec3& w) noexcept
{ 
	return glm::max(0.f, 1.f - cos_theta_sq(w));
}

inline float sin_theta(const glm::vec3& w) noexcept 
{ 
	return glm::sqrt(sin_theta_sq(w));
}

inline float tan_theta(const glm::vec3& w) noexcept 
{ 
	return sin_theta(w) / cos_theta(w);
}

inline float tan_theta_sq(const glm::vec3& w) noexcept 
{ 
	return sin_theta_sq(w) / cos_theta_sq(w);
}

inline bool on_same_hemisphere(const glm::vec3& wi, const glm::vec3& wo) noexcept
{
	return wi.z * wo.z > 0.f;
}

// ggx
inline float D_GGX(const glm::vec3& wh, float alpha) noexcept
{
	float alphaSq = alpha * alpha;
	float a = 1.0f + cos_theta_sq(wh) * (alphaSq - 1.0f);
	return alphaSq / (std::numbers::pi_v<float> * a * a);
}

inline float G1_Smith_GGX(const glm::vec3& w, float alpha) noexcept
{
	float tanTheraSq = tan_theta_sq(w);
	if (std::isinf(tanTheraSq))
		return 0.f;

	float alphaSq = alpha * alpha;
	float lambda = (-1.f + glm::sqrt(alphaSq * tanTheraSq + 1.f)) / 2.f;
	return 1.f / (1.f + lambda);
}

inline float G2_Smith_Height_Correlated_GGX(const glm::vec3& wi, const glm::vec3& wo, float alpha) 
{
	float tan2ThetaO = tan_theta_sq(wo);
	if (std::isinf(tan2ThetaO) || std::isnan(tan2ThetaO))
		return 0.0f;

	float tan2ThetaI = tan_theta_sq(wi);
	if (std::isinf(tan2ThetaI) || std::isnan(tan2ThetaI)) 
		return 0.0f;

	float alpha2 = alpha * alpha;
	assert(alpha2 * tan2ThetaO >= -1.0f);
	assert(alpha2 * tan2ThetaI >= -1.0f);
	float lambda_wo = (-1.0f + glm::sqrt(alpha2 * tan2ThetaO + 1.0f)) / 2.0f;
	float lambda_wi = (-1.0f + glm::sqrt(alpha2 * tan2ThetaI + 1.0f)) / 2.0f;
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

inline glm::vec3 microfacet_reflection_ggx(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& f0, float eta, float alpha, glm::vec3& F)
{
	if (!on_same_hemisphere(wi, wo) || cos_theta(wi) == 0.0f || cos_theta(wo) == 0.0f)
		return glm::vec3(0.0f);

	glm::vec3 wh = wi + wo;
	if (wh.x == 0.0f && wh.y == 0.0f && wh.z == 0.0f)
		return glm::vec3(0.0f);

	wh = glm::normalize(wh);

	if (eta < 1.0f) 
	{
		float cosThetaT = glm::dot(wi, wh);
		float cos2ThetaT = cosThetaT * cosThetaT;
		F = cos2ThetaT > 0.0f ? F_Schlick(glm::abs(cosThetaT), f0) : glm::vec3(1.0f);
	}
	else 
		F = F_Schlick(glm::abs(glm::dot(wh, wo)), f0);

	float G = G2_Smith_Height_Correlated_GGX(wi, wo, alpha);
	float D = D_GGX(wh, alpha);
	return F * G * D / (4.0f * glm::abs(cos_theta(wi)) * glm::abs(cos_theta(wo)));
}

inline glm::vec3 microfacet_transmission_ggx(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& f0, float eta, float alpha)
{
	if (on_same_hemisphere(wi, wo) || cos_theta(wi) == 0.0f || cos_theta(wo) == 0.0f)
		return glm::vec3(0.0f);

	glm::vec3 wh = glm::normalize(wi + wo * eta);
	if (cos_theta(wh) < 0.0f)
		wh = -wh;
	
	float cosThetaI = glm::dot(wo, wh);
	float cosThetaT = glm::dot(wi, wh);
	if (cosThetaI * cosThetaT > 0.0f)
		return glm::vec3(0.0f);

	glm::vec3 F;
	if (eta < 1.0f) 
	{
		float cos2ThetaT = cosThetaT * cosThetaT;
		F = cos2ThetaT > 0.0f ? F_Schlick(glm::abs(cosThetaT), f0) : glm::vec3(1.0f);
	}
	else
		F = F_Schlick(glm::abs(glm::dot(wh, wo)), f0);

	float G = G2_Smith_Height_Correlated_GGX(wi, wo, alpha);
	float D = D_GGX(wh, alpha);
	float denomSqrt = cosThetaT + eta * cosThetaI;
	return (1.f - F) * D * G * glm::abs(cosThetaT) * glm::abs(cosThetaI) / (denomSqrt * denomSqrt * glm::abs(cos_theta(wi)) * glm::abs(cos_theta(wo)));
}

inline float cosine_weighted_pdf(const glm::vec3& wi, const glm::vec3& wo) noexcept
{
	return on_same_hemisphere(wi, wo) ? cos_theta(wi) / std::numbers::pi_v<float> : 0.f;
}

inline float ggx_vndf_reflection_pdf(const glm::vec3& wi, const glm::vec3& wo, float alpha) noexcept
{
	if (!on_same_hemisphere(wi, wo))
		return 0.f;

	glm::vec3 wh = glm::normalize(wi + wo);
	float pdf_h = G1_Smith_GGX(wo, alpha) * D_GGX(wh, alpha) * glm::abs(glm::dot(wh, wo)) / glm::abs(cos_theta(wo));
	float dwh_dwi = 1.0f / (4.0f * glm::dot(wi, wh));
	return pdf_h * dwh_dwi;
}

inline float ggx_vndf_transmission_pdf(const glm::vec3& wi, const glm::vec3& wo, float eta, float alpha) noexcept
{
	if (on_same_hemisphere(wi, wo))
		return 0.f;

	glm::vec3 wh = glm::normalize(wi + eta * wo);

	float cosThetaI = glm::dot(wo, wh);
	float cosThetaT = glm::dot(wi, wh);
	if (cosThetaI * cosThetaT > 0.0f)
		return 0.0f;

	float pdf_h = G1_Smith_GGX(wo, alpha) * D_GGX(wh, alpha) * glm::abs(cosThetaI) / glm::abs(cos_theta(wo));
	float sqrtDenom = cosThetaT + eta * cosThetaI;
	float dwh_dwi = glm::abs(cosThetaT) / (sqrtDenom * sqrtDenom);
	return pdf_h * dwh_dwi;
}