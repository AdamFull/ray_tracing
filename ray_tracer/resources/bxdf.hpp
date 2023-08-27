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
inline float d_ggx(const glm::vec3& wh, float alpha) noexcept
{
	float alpha_sq = alpha * alpha;
	float a = 1.f + cos_theta_sq(wh) * (alpha_sq - 1.f);
	return alpha_sq / (std::numbers::pi_v<float> * a * a);
}

inline float g1_smith_ggx(const glm::vec3& w, float alpha) noexcept
{
	float tan_sq = tan_theta_sq(w);

	if (std::isinf(tan_sq))
		return 0.f;

	float alpha2 = alpha * alpha;
	assert(alpha2 * tan_sq >= -1.0f);
	float lambda = (-1.f + std::sqrt(alpha2 * tan_sq + 1.f)) / 2.f;

	return 1.f / (1.f + lambda);
}

inline float g2_smith_height_correlated_ggx(const glm::vec3& wi, const glm::vec3& wo, float alpha) 
{
	float tan2ThetaO = tan_theta_sq(wo);
	float tan2ThetaI = tan_theta_sq(wi);

	if (std::isinf(tan2ThetaO)) 
		return 0.f;

	if (std::isinf(tan2ThetaI)) 
		return 0.f;

	float alpha2 = alpha * alpha;
	assert(alpha2 * tan2ThetaO >= -1.0f);
	assert(alpha2 * tan2ThetaI >= -1.0f);
	float lambda_wo = (-1.f + glm::sqrt(alpha2 * tan2ThetaO + 1.f)) / 2.f;
	float lambda_wi = (-1.f + glm::sqrt(alpha2 * tan2ThetaI + 1.f)) / 2.f;
	return 1.f / (1.f + lambda_wo + lambda_wi);
}

inline glm::vec3 f_schlick_eta(float eta) noexcept
{
	float a = (1.f - eta) / (1.f + eta);
	return glm::vec3(a * a);
}

inline glm::vec3 f_schlick(float cosThetaI, const glm::vec3& f0) noexcept
{
	float a = glm::max(0.f, 1.f - cosThetaI);
	float a2 = a * a;
	float a5 = a2 * a2 * a;
	return f0 + (glm::vec3(1.f) - f0) * a5;
}

inline glm::vec3 lambert_diffuse(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& diffuse_color)
{
	if (!on_same_hemisphere(wi, wo))
		return glm::vec3(0.f);

	return diffuse_color / std::numbers::pi_v<float>;
}

inline glm::vec3 microfacet_reflection_ggx(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& f0, float eta, float alpha)
{
	if (!on_same_hemisphere(wi, wo) || math::compare_float(cos_theta(wi), 0.f) || math::compare_float(cos_theta(wo), 0.f))
		return glm::vec3(0.f);

	glm::vec3 wh = wi + wo;
	if (math::compare_float(wh.x, 0.f) && math::compare_float(wh.y, 0.f) && math::compare_float(wh.z, 0.f)) 
		return glm::vec3(0.f);

	wh = glm::normalize(wh);

	glm::vec3 F{};

	if (eta < 1.f)
	{
		float cosThetaT = glm::dot(wi, wh);
		float cos2ThetaT = cosThetaT * cosThetaT;
		F = cos2ThetaT > 0.f ? f_schlick(glm::abs(cosThetaT), f0) : glm::vec3(1.f);
	}
	else
		F = f_schlick(glm::abs(glm::dot(wh, wo)), f0);

	float G = g2_smith_height_correlated_ggx(wi, wo, alpha);
	float D = d_ggx(wh, alpha);
	return F * G * D / (4.f * glm::abs(cos_theta(wi)) * glm::abs(cos_theta(wo)));
}

inline glm::vec3 microfacet_transmission_ggx(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& f0, float eta, float alpha)
{
	if (on_same_hemisphere(wi, wo) || math::compare_float(cos_theta(wi), 0.f) || math::compare_float(cos_theta(wo), 0.f))
		return glm::vec3(0.f);

	glm::vec3 wh = glm::normalize(wi + eta * wo);
	if (cos_theta(wh) < 0.f)
		wh = -wh;

	if (glm::dot(wo, wh) * glm::dot(wi, wh) > 0.f)
		return glm::vec3(0.f);

	glm::vec3 F;
	if (eta < 1.f) 
	{
		float cosThetaT = glm::dot(wi, wh);
		float cos2ThetaT = cosThetaT * cosThetaT;
		F = cos2ThetaT > 0.f ? f_schlick(glm::abs(cosThetaT), f0) : glm::vec3(1.f);
	}
	else
		F = f_schlick(abs(dot(wh, wo)), f0);

	float G = g2_smith_height_correlated_ggx(wi, wo, alpha);
	float D = d_ggx(wh, alpha);
	float denomSqrt = dot(wi, wh) + eta * dot(wo, wh);
	return (glm::vec3(1.f) - F) * D * G * glm::abs(glm::dot(wi, wh)) * glm::abs(glm::dot(wo, wh)) / (denomSqrt * denomSqrt * glm::abs(cos_theta(wi)) * glm::abs(cos_theta(wo)));
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
	float pdf_h = g1_smith_ggx(wo, alpha) * d_ggx(wh, alpha) * glm::abs(glm::dot(wh, wo)) / glm::abs(cos_theta(wo));
	float dwh_dwi = 1.f / (4.f * glm::dot(wi, wh));
	return pdf_h * dwh_dwi;
}

inline float ggx_vndf_refraction_pdf(const glm::vec3& wi, const glm::vec3& wo, float eta, float alpha) noexcept
{
	if (on_same_hemisphere(wi, wo))
		return 0.f;

	glm::vec3 wh = glm::normalize(wi + wo);

	bool on_same_side = glm::dot(wo, wh) * glm::dot(wi, wh) > 0.f;
	if (on_same_side) 
		return 0.f;

	float pdf_h = g1_smith_ggx(wo, alpha) * d_ggx(wh, alpha) * glm::abs(glm::dot(wh, wo)) / glm::abs(cos_theta(wo));
	float sqrt_denom = glm::dot(wi, wh) + eta * glm::dot(wo, wh);
	float dwh_dwi = glm::abs(glm::dot(wi, wh)) / (sqrt_denom * sqrt_denom);
	return pdf_h * dwh_dwi;
}