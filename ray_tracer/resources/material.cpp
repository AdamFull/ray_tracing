#include "resource_manager.h"
#include "util.h"

#include "ggx.hpp"

glm::vec3 sample_hemisphere(const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal)
{
	float u = random<float>();
	float v = random<float>();

	float theta = 2 * std::numbers::pi_v<float> * u;
	float phi = std::acos(2.f * v - 1.f);

	glm::vec3 direction(glm::sin(phi) * glm::cos(theta), glm::sin(phi) * glm::sin(theta), glm::cos(phi));

	direction = math::apply_otb(tangent, bitangent, normal, direction);
	return direction;
}

glm::vec3 calculate_tangent_space_normal(const glm::vec3 normal_color, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal, float scale)
{
	return math::normalize(math::apply_otb(tangent, bitangent, normal, ((2.f * normal_color - 1.f) * glm::vec3(scale, scale, 1.0))));
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
	if (m_textures.count(ETextureType::eNormal))
		return normal;
	
	auto sampled_normal = glm::vec3(sample_texture(ETextureType::eNormal, uv));

	return calculate_tangent_space_normal(sampled_normal, tangent, bitangent, normal, 1.f);
}


// Lambertian material
CLambertianMaterial::CLambertianMaterial(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CLambertianMaterial::create(const FMaterialCreateInfo& createInfo)
{
	m_textures = createInfo.m_textures;
	m_albedo = glm::vec3(createInfo.m_baseColorFactor);
}

bool CLambertianMaterial::scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf, float& alpha) const
{
	// Sampling tangent space normal
	auto ts_normal = sample_tangent_space_normal(hit_result.m_texcoord, hit_result.m_tangent, hit_result.m_bitangent, hit_result.m_normal);

	auto albedo = m_albedo * hit_result.m_color;
	if (m_textures.count(ETextureType::eAlbedo))
	{
		auto color = sample_texture(ETextureType::eAlbedo, hit_result.m_texcoord);
		albedo *= glm::vec3(color);
		alpha = color.a;
	}

	out_ray.m_origin = hit_result.m_position;
	out_ray.m_origin = hit_result.m_position;
	if (m_alphaMode == EAlphaMode::eMask && alpha < m_alphaCutoff)
	{
		out_ray.set_direction(in_ray.m_direction);
		return true;
	}
	else
		out_ray.m_direction = ts_normal + sample_hemisphere(hit_result.m_tangent, hit_result.m_bitangent, hit_result.m_normal);

	pdf = math::dot(ts_normal, out_ray.m_direction) / std::numbers::pi_v<float>;

	color = albedo;

	return true;
}


CMetalRoughnessMaterial::CMetalRoughnessMaterial(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CMetalRoughnessMaterial::create(const FMaterialCreateInfo& createInfo)
{
	m_textures = createInfo.m_textures;
	m_albedo = glm::vec3(createInfo.m_baseColorFactor);
	m_metallic = createInfo.m_fMetallicFactor;
	m_roughness = createInfo.m_fRoughnessFactor;
	m_alphaMode = createInfo.m_alphaMode;
	m_alphaCutoff = createInfo.m_alphaCutoff;
}

bool CMetalRoughnessMaterial::scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf, float& alpha) const
{
	// Sampling tangent space normal
	auto ts_normal = sample_tangent_space_normal(hit_result.m_texcoord, hit_result.m_tangent, hit_result.m_bitangent, hit_result.m_normal);

	// Sampling metallic roughness texture
	float metallic = m_metallic;
	float roughness = m_roughness;
	if (m_textures.count(ETextureType::eMetallRoughness))
	{
		auto sampled_mr = sample_texture(ETextureType::eMetallRoughness, hit_result.m_texcoord);
	
		roughness = roughness * sampled_mr.y;
		metallic = metallic * sampled_mr.z;
		roughness = glm::clamp(roughness, 0.04f, 1.f);
	}

	// Sampling albedo texture
	auto albedo = m_albedo * hit_result.m_color;
	if (m_textures.count(ETextureType::eAlbedo))
	{
		auto color = sample_texture(ETextureType::eAlbedo, hit_result.m_texcoord);
		albedo *= glm::vec3(color);
		alpha = color.a;
	}

	out_ray.m_origin = hit_result.m_position;
	if (alpha < random<float>())
	{
		out_ray.set_direction(in_ray.m_direction);
		return true;
	}
	else
		out_ray.set_direction(importance_sample_ggx(glm::vec2(random<float>(), random<float>()), roughness, ts_normal));

	glm::vec3 V = in_ray.m_direction * -1.f;

	return calculate_cook_torrance_color_brdf(color, out_ray.m_direction, V, ts_normal, albedo, roughness, metallic);
}

float CMetalRoughnessMaterial::scatter_pdf(const FRay& in_ray, const FHitResult& hit_result, const FRay& out_ray) const
{
	auto cosine = math::dot(hit_result.m_normal, out_ray.m_direction);
	return cosine < 0.f ? 0.f : cosine / std::numbers::pi_v<float>;
}


// Emissive material
CEmissiveMaterial::CEmissiveMaterial(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CEmissiveMaterial::create(const FMaterialCreateInfo& createInfo)
{
	m_textures = createInfo.m_textures;
	m_emissive = createInfo.m_emissiveFactor;
	m_emissionStrength = createInfo.m_emissiveStrength;
}

glm::vec3 CEmissiveMaterial::emit(const FHitResult& hit_result) const
{
	glm::vec3 emission{ m_emissive };
	if (m_textures.count(ETextureType::eEmission))
		emission *= glm::vec3(sample_texture(ETextureType::eEmission, hit_result.m_texcoord));

	return emission * m_emissionStrength;
}

// Dielectric material
void CDielectricMaterial::create(const FMaterialCreateInfo& createInfo)
{

}

bool CDielectricMaterial::scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf, float& alpha) const
{
	float refraction_ratio = hit_result.m_bFrontFace ? (1.f / m_fIor) : m_fIor;

	auto direction = math::normalize(in_ray.m_direction);
	float cos_theta = glm::min(math::dot(-direction, hit_result.m_normal), 1.f);
	float sin_theta = math::fsqrt(1.f - cos_theta * cos_theta);

	if ((refraction_ratio * sin_theta > 1.f) || reflectance(cos_theta, refraction_ratio) > random<float>())
		out_ray.set_direction(glm::reflect(direction, hit_result.m_normal));
	else
		out_ray.set_direction(glm::refract(direction, hit_result.m_normal, refraction_ratio));

	out_ray.m_origin = hit_result.m_position;
	return true;
}

float CDielectricMaterial::reflectance(float cosine, float ref_idx)
{
	auto r0 = (1.f - ref_idx) / (1.f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.f - r0) * std::pow((1.f - cosine), 5.f);
}