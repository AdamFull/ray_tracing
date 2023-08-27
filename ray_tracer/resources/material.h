#pragma once

#include <shared.h>

enum class EAlphaMode
{
	eOpaque,
	eMask,
	eBlend
};

enum class ETextureType
{
	eAlbedo,
	eMetallRoughness,
	eNormal,
	eAmbientOcclusion,
	eEmission,
	eBRDFLut
};

struct FMaterialCreateInfo
{
	std::unordered_map<ETextureType, resource_id_t> m_textures;

	EAlphaMode m_alphaMode{ EAlphaMode::eOpaque };
	float m_alphaCutoff{ 0.5f };

	float m_fNormalMapScale{ 1.f };
	float m_fOcclusionStrength{ 1.f };

	glm::vec4 m_baseColorFactor{ 0.f };

	glm::vec3 m_emissiveFactor{ 0.f };
	float m_emissiveStrength{ 1.f };

	float m_fRoughnessFactor{ 1.f };
	float m_fMetallicFactor{ 1.f };
};

class COrthonormalBasis
{
public:
	COrthonormalBasis(const glm::vec3& u, const glm::vec3& v, const glm::vec3& w) : m_u(u), m_v(v), m_w(w) {}

	COrthonormalBasis(const glm::vec3& n)
	{
		const float sign = std::copysign(1.f, n.z);
		const float a = -1.0f / (sign + n.z);
		const float b = n.x * n.y * a;
		m_u = glm::vec3(1.f + sign * n.x * n.x * a, sign * b, -sign * n.x);
		m_v = glm::vec3(b, sign + n.y * n.y * a, -n.y);
		m_w = n;
	}

	glm::vec3 to_world(const glm::vec3& vec) const
	{
		return glm::vec3(
			vec.x * m_u.x + vec.y * m_v.x + vec.z * m_w.x,
			vec.x * m_u.y + vec.y * m_v.y + vec.z * m_w.y,
			vec.x * m_u.z + vec.y * m_v.z + vec.z * m_w.z
		);
	}

	glm::vec3 to_local(const glm::vec3& vec) const
	{
		return glm::vec3(glm::dot(vec, m_u), glm::dot(vec, m_v), glm::dot(vec, m_w));
	}
private:
	glm::vec3 m_u, m_v, m_w;
};

class CResourceManager;

class CMaterial
{
public:
	CMaterial() = default;
	CMaterial(CResourceManager* resource_manager) : m_pResourceManager(resource_manager) {}
	~CMaterial() {}

	void create(const FMaterialCreateInfo& createInfo);

	glm::vec3 emit(const FHitResult& hit_result) const;
	bool can_emit_light() const;
	bool can_scatter_light() const;

	glm::vec3 sample(const glm::vec3& wo, const glm::vec3& color, float metallic, float alpha, float transmission, float s, float t, float& pdf) const;
	glm::vec3 eval(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& color, float metallic, float alpha, float transmittance) const;
	float pdf(const glm::vec3& wi, const glm::vec3& wo, const glm::vec3& color, float metallic, float alpha, float transmission) const;

	glm::vec3 sample_diffuse_color(const FHitResult& hit_result);
	glm::vec3 sample_surface_normal(const FHitResult& hit_result) const;
	glm::vec2 sample_surface_metallic_roughness(const FHitResult& hit_result) const;
	
protected:
	glm::vec4 sample_texture(ETextureType texture, const glm::vec2& uv) const;
	glm::vec3 sample_tangent_space_normal(const glm::vec2& uv, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal) const;

	void compute_lobe_probabilities(const glm::vec3& wo, const glm::vec3& color, float metallic, float transmission, const float& eta, float& diffuse, float& specular, float& transmittance) const;
protected:
	std::unordered_map<ETextureType, resource_id_t> m_textures{};

	glm::vec3 m_albedo{};

	float m_metallic{ 1.f };
	float m_roughness{ 1.f };

	glm::vec3 m_emissive{};
	float m_emissionStrength{ 1.f };

	float m_ior{ 1.f };
	float m_transmission{ 0.f };

	EAlphaMode m_alphaMode{};
	float m_alphaCutoff{};
	CResourceManager* m_pResourceManager{ nullptr };
};