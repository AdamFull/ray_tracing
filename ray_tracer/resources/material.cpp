#include "resource_manager.h"
#include "util.h"

#include "bxdf.hpp"

#include "render/rsampler.h"

glm::vec3 srgb_to_linear(glm::vec3 srgbIn)
{
	glm::vec3 bLess = glm::step(glm::vec3(0.04045f), srgbIn);
	return glm::mix(srgbIn / glm::vec3(12.92f), glm::pow((srgbIn + glm::vec3(0.055f)) / glm::vec3(1.055f), glm::vec3(2.4f)), bLess);
}

void CMaterial::create(const FMaterialCreateInfo& createInfo)
{
	m_textures = createInfo.m_textures;
	m_albedo = glm::vec3(createInfo.m_baseColorFactor);
	m_metallic = createInfo.m_fMetallicFactor;
	m_roughness = createInfo.m_fRoughnessFactor;
	m_emissive = createInfo.m_emissiveFactor;
	m_emissionStrength = createInfo.m_emissiveStrength;
	m_alphaMode = createInfo.m_alphaMode;
	m_alphaCutoff = createInfo.m_alphaCutoff;

	m_ior = createInfo.m_fIor;
	m_transmission = createInfo.m_fTransmission;
}

glm::vec3 CMaterial::emit(const FHitResult& hit_result) const
{
	glm::vec3 emission{ m_emissive };
	if (m_textures.count(ETextureType::eEmission))
		emission *= srgb_to_linear(glm::vec3(sample_texture(ETextureType::eEmission, hit_result.m_texcoord)));

	return emission * m_emissionStrength;
}

bool CMaterial::can_emit_light() const
{
	return m_emissive != glm::vec3(0.f) || m_textures.count(ETextureType::eEmission);
}

bool CMaterial::can_scatter_light() const
{
	return m_albedo != glm::vec3(0.f) || m_textures.count(ETextureType::eAlbedo) || m_textures.count(ETextureType::eMetallRoughness);
}

glm::vec3 CMaterial::sample(const glm::vec3& wo, const glm::vec3& color, float metallic, float alpha, float u, float v, float& pdf) const
{
	float eta = cos_theta(wo) > 0.f ? 1.f / m_ior : m_ior;

	float diffuse_weight, specular_weight, transmittance_weight;
	compute_lobe_probabilities(wo, color, metallic, eta, diffuse_weight, specular_weight, transmittance_weight);

	glm::vec3 wi{};

	if (u < diffuse_weight)
	{
		u = math::remap(u, 0.0f, diffuse_weight - std::numeric_limits<float>::epsilon(), 0.0f, 1.f - std::numeric_limits<float>::epsilon());
		assert(u >= 0.0f && u < 1.0f);

		wi = math::sign(cos_theta(wo)) * CCMGSampler::sample_cosine_hemisphere(u, v);
		assert(math::is_normalized(wi));
	}
	else if (u < diffuse_weight + specular_weight)
	{
		u = math::remap(u, diffuse_weight, diffuse_weight + specular_weight - std::numeric_limits<float>::epsilon(), 0.0f, 1.f - std::numeric_limits<float>::epsilon());
		assert(u >= 0.0f && u < 1.0f);

		glm::vec3 wo_upper = math::sign(cos_theta(wo)) * wo;
		glm::vec3 wh = math::sign(cos_theta(wo)) * CCMGSampler::sample_ggx_vndf(wo_upper, alpha, u, v);
		if (glm::dot(wo, wh) < 0.0f)
			return glm::vec3(0.0f);

		wi = glm::reflect(wo, wh);
		if (!on_same_hemisphere(wi, wo))
			return glm::vec3(0.0f);
	}
	else
	{
		u = math::remap(u, diffuse_weight + specular_weight, 1.f - std::numeric_limits<float>::epsilon(), 0.0f, 1.f - std::numeric_limits<float>::epsilon());
		assert(u >= 0.0f && u < 1.0f);

		glm::vec3 wo_upper = math::sign(cos_theta(wo)) * wo;
		glm::vec3 wh = math::sign(cos_theta(wo)) * CCMGSampler::sample_ggx_vndf(wo_upper, alpha, u, v);
		if (glm::dot(wo, wh) < 0.0f)
			return glm::vec3(0.0f);

		if (!math::refract(wo, wh, eta, wi))
			return glm::vec3(0.0f);

		if (on_same_hemisphere(wi, wo))
			return glm::vec3(0.0f);

		if (glm::dot(wo, wh) * glm::dot(wi, wh) > 0.0f)
			return glm::vec3(0.0f);
	}

	pdf = diffuse_weight * cosine_weighted_pdf(wi, wo) + specular_weight * ggx_vndf_reflection_pdf(wi, wo, alpha) + transmittance_weight * ggx_vndf_transmission_pdf(wi, wo, eta, alpha);

	return wi;
}

glm::vec3 CMaterial::eval(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& color, float metallic, float alpha) const
{
	float eta = cos_theta(wo) > 0.f ? 1.f / m_ior : m_ior;
	glm::vec3 f0 = glm::mix(f_schlick_eta(eta), color, metallic);

	glm::vec3 diffuse = lambert_diffuse(wi, wo, color);
	glm::vec3 specular = microfacet_reflection_ggx(wi, wo, f0, eta, alpha);
	glm::vec3 transmission = color * microfacet_transmission_ggx(wi, wo, f0, eta, alpha);

	float diffuse_weight = (1.f - metallic) * (1.f - m_transmission);
	float transmission_weight = (1.f - metallic) * m_transmission;

	return diffuse_weight * diffuse + specular + transmission_weight * transmission;
}

float CMaterial::pdf(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& color, float metallic, float alpha) const
{
	float eta = cos_theta(wo) > 0.f ? 1.f / m_ior : m_ior;
	float diffuse_weight, specular_weight, transmittance_weight;
	compute_lobe_probabilities(wo, color, metallic, eta, diffuse_weight, specular_weight, transmittance_weight);

	return diffuse_weight * cosine_weighted_pdf(wi, wo) + specular_weight * ggx_vndf_reflection_pdf(wi, wo, alpha) + transmittance_weight * ggx_vndf_transmission_pdf(wi, wo, eta, alpha);
}

glm::vec3 CMaterial::sample_surface_normal(const FHitResult& hit_result) const
{
	return sample_tangent_space_normal(hit_result.m_texcoord, hit_result.m_tangent, hit_result.m_bitangent, hit_result.m_normal);
}

glm::vec3 CMaterial::sample_diffuse_color(const FHitResult& hit_result)
{
	auto diffuse = m_albedo * hit_result.m_color;
	if (m_textures.count(ETextureType::eAlbedo))
	{
		auto color = sample_texture(ETextureType::eAlbedo, hit_result.m_texcoord);
		diffuse *= srgb_to_linear(glm::vec3(color));
	}
	return diffuse;
}

glm::vec2 CMaterial::sample_surface_metallic_roughness(const FHitResult& hit_result) const
{
	glm::vec2 mr{ m_metallic, m_roughness };
	if (m_textures.count(ETextureType::eMetallRoughness))
	{
		auto sampled_mr = glm::vec3(sample_texture(ETextureType::eMetallRoughness, hit_result.m_texcoord));

		mr.y = mr.y * sampled_mr.y;
		mr.y = glm::max(0.001f, mr.y * mr.y);
		mr.x = mr.x > 0.f ? mr.x * sampled_mr.z : sampled_mr.z;
	}

	return mr;
}

glm::vec4 CMaterial::sample_texture(ETextureType texture, const glm::vec2& uv) const
{
	auto found_texture = m_textures.find(texture);
	if (found_texture != m_textures.end())
	{
		auto& image = m_pResourceManager->get_image(found_texture->second);
		auto& sampler = m_pResourceManager->get_sampler(image->get_sampler());

		return sampler->sample(found_texture->second, uv);
	}

	return glm::vec4(0.f);
}

glm::vec3 CMaterial::sample_tangent_space_normal(const glm::vec2& uv, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal) const
{
	if (!m_textures.count(ETextureType::eNormal))
		return normal;

	auto sampled_normal = glm::vec3(sample_texture(ETextureType::eNormal, uv));

	if (math::isnan(tangent) || math::isnan(bitangent))
	{
		COrthonormalBasis otb(normal);
		return glm::normalize(otb.to_local(((2.f * sampled_normal - 1.f) * glm::vec3(1.f, 1.f, 1.f))));
	}
	
	return glm::normalize(glm::mat3(tangent, bitangent, normal) * ((2.f * sampled_normal - 1.f) * glm::vec3(1.f, 1.f, 1.f)));
}

void CMaterial::compute_lobe_probabilities(const glm::vec3& wo, const glm::vec3& color, float metallic, const float& eta, float& diffuse, float& specular, float& transmittance) const
{
	glm::vec3 f0 = glm::mix(f_schlick_eta(eta), color, metallic);
	glm::vec3 fresnel = f_schlick(glm::abs(cos_theta(wo)), f0);

	float diffuse_weight = (1.f - metallic) * (1.f - m_transmission);
	float transmission_weight = (1.f - metallic) * m_transmission;

	diffuse = math::max_component(color) * diffuse_weight;
	specular = math::max_component(fresnel);
	transmittance = math::max_component(glm::vec3(1.f) - fresnel) * transmission_weight;

	float factor = 1.f / (diffuse + specular + transmittance);
	diffuse *= factor;
	specular *= factor;
	transmittance *= factor;
}