#include "resource_manager.h"
#include "util.h"

glm::vec3 F_SchlickR(float cosTheta, const glm::vec3& F0, float roughness)
{
	return F0 + (glm::max(glm::vec3(1.0f - roughness), F0) - F0) * glm::pow(1.0f - cosTheta, 5.0f);
}

glm::mat3 calculate_tbn(const glm::vec3& tangent, const glm::vec3& bitangent, glm::vec3& normal)
{
	return glm::mat3{ tangent, bitangent, normal };
}

glm::vec3 calculate_tangent_space_normal(const glm::vec3 normal_color, const glm::mat3& tbn, float scale)
{
	return glm::normalize(tbn * ((2.f * normal_color - 1.f) * glm::vec3(scale, scale, 1.0)));
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

	return calculate_tangent_space_normal(sampled_normal, glm::mat3(tangent, bitangent, normal), 1.f);
}



CDiffuseMaterial::CDiffuseMaterial(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CDiffuseMaterial::create(const FMaterialCreateInfo& createInfo)
{
	m_textures = createInfo.m_textures;
	m_albedo = glm::vec3(createInfo.m_baseColorFactor);
	m_emissive = createInfo.m_emissiveFactor;
	m_metallic = createInfo.m_fMetallicFactor;
	m_roughness = createInfo.m_fRoughnessFactor;
}

bool CDiffuseMaterial::scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const
{
	// Sampling tangent space normal
	auto ts_normal = sample_tangent_space_normal(hit_result.m_texcoord, hit_result.m_tangent, hit_result.m_bitangent, hit_result.m_normal);

	// Sampling metallic roughness texture
	float metallic = m_metallic;
	float roughness = m_roughness;
	if (m_textures.count(ETextureType::eMetallRoughness))
	{
		auto sampled_mr = sample_texture(ETextureType::eMetallRoughness, hit_result.m_texcoord);
	
		roughness = roughness * sampled_mr.g;
		metallic = metallic * sampled_mr.b;
		roughness = glm::clamp(roughness, 0.04f, 1.f);
	}

	out_ray.m_origin = hit_result.m_position;
	out_ray.m_direction = glm::reflect(glm::normalize(in_ray.m_direction), ts_normal + roughness * glm::normalize(random_unit_vec3<float>()));

	// Sampling albedo texture
	auto albedo = m_albedo * hit_result.m_color;
	if(m_textures.count(ETextureType::eAlbedo))
		albedo *= glm::vec3(sample_texture(ETextureType::eAlbedo, hit_result.m_texcoord));

	glm::vec3 F0{ 0.04f };
	F0 = glm::mix(F0, albedo, metallic);

	float cosine = glm::dot(glm::normalize(out_ray.m_direction), glm::normalize(ts_normal));
	glm::vec3 F = F_SchlickR(glm::max(cosine, 0.0f), F0, roughness);

	glm::vec3 kD = 1.f - F;
	kD *= 1.f - metallic;

	color = albedo;

	return glm::dot(out_ray.m_direction, ts_normal) > 0.f;
}

glm::vec3 CDiffuseMaterial::emit(const FHitResult& hit_result) const
{
	if (!hit_result.m_bFrontFace)
		return glm::vec3(0.f);

	glm::vec3 emission{ 0.f };
	if (m_textures.count(ETextureType::eEmission))
		emission = m_emissive * glm::vec3(sample_texture(ETextureType::eEmission, hit_result.m_texcoord));
	emission *= m_emissionStrength;

	return emission;
}


void CMetallRoughnessMaterial::create(const FMaterialCreateInfo& createInfo)
{

}

bool CMetallRoughnessMaterial::scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const
{
	out_ray.m_origin = hit_result.m_position;
	out_ray.m_direction = glm::reflect(glm::normalize(in_ray.m_direction), hit_result.m_normal) + m_fRoughness * glm::normalize(random_unit_vec3<float>());
	color *= m_albedo;
	return glm::dot(out_ray.m_direction, hit_result.m_normal) > 0.f;
}


void CDielectricMaterial::create(const FMaterialCreateInfo& createInfo)
{

}

bool CDielectricMaterial::scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const
{
	float refraction_ratio = hit_result.m_bFrontFace ? (1.f / m_fIor) : m_fIor;

	auto direction = glm::normalize(in_ray.m_direction);
	float cos_theta = std::fmin(glm::dot(-direction, hit_result.m_normal), 1.f);
	float sin_theta = glm::sqrt(1.f - cos_theta * cos_theta);

	if ((refraction_ratio * sin_theta > 1.f) || reflectance(cos_theta, refraction_ratio) > random<float>())
		out_ray.m_direction = glm::reflect(direction, hit_result.m_normal);
	else
		out_ray.m_direction = glm::refract(direction, hit_result.m_normal, refraction_ratio);

	out_ray.m_origin = hit_result.m_position;
	return true;
}

float CDielectricMaterial::reflectance(float cosine, float ref_idx)
{
	auto r0 = (1.f - ref_idx) / (1.f + ref_idx);
	r0 = r0 * r0;
	return r0 + (1.f - r0) * std::pow((1.f - cosine), 5.f);
}