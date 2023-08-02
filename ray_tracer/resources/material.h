#pragma once

#include <shared.h>

enum class ETextureType
{
	eAlbedo,
	eMetallRoughness,
	eNormal,
	eAmbientOcclusion,
	eEmission
};

struct FMaterialCreateInfo
{
	std::unordered_map<ETextureType, resource_id_t> m_textures;

	float m_fNormalMapScale{ 1.f };
	float m_fOcclusionStrength{ 1.f };

	glm::vec4 m_baseColorFactor{ 1.f };

	glm::vec3 m_emissiveFactor{ 1.f };

	float m_fRoughnessFactor{ 1.f };
	float m_fMetallicFactor{ 1.f };
};

class CResourceManager;

class CMaterial
{
public:
	CMaterial() = default;
	virtual ~CMaterial() {}

	virtual glm::vec3 emit(const FHitResult& hit_result) const
	{
		return glm::vec3(0.f);
	}

	virtual void create(const FMaterialCreateInfo& createInfo) = 0;
	virtual bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const = 0;
protected:
	glm::vec4 sample_texture(ETextureType texture, const glm::vec2& uv) const;
	glm::vec3 sample_tangent_space_normal(const glm::vec2& uv, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal) const;
protected:
	std::unordered_map<ETextureType, resource_id_t> m_textures{};
	CResourceManager* m_pResourceManager{ nullptr };
};

class CDiffuseMaterial : public CMaterial
{
public:
	CDiffuseMaterial() = default;
	CDiffuseMaterial(CResourceManager* resource_manager);
	CDiffuseMaterial(const glm::vec3& albedo) : m_albedo(albedo) {}
	~CDiffuseMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const override;
	glm::vec3 emit(const FHitResult& hit_result) const override;
private:
	
	glm::vec3 m_albedo{};
	glm::vec3 m_emissive{};
	float m_emissionStrength{ 2.f };
	float m_metallic{ 1.f };
	float m_roughness{ 1.f };
};

class CMetallRoughnessMaterial : public CMaterial
{
public:
	CMetallRoughnessMaterial() = default;
	CMetallRoughnessMaterial(const glm::vec3& albedo, float roughness) : m_albedo(albedo), m_fRoughness(roughness < 1.f ? roughness : 1.f) {}
	~CMetallRoughnessMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const override;
private:
	glm::vec3 m_albedo{};
	float m_fRoughness{ 1.f };
	float m_fMetallic{ 1.f };
};

class CDielectricMaterial : public CMaterial
{
public:
	CDielectricMaterial() = default;
	CDielectricMaterial(float ior) : m_fIor(ior) {}
	~CDielectricMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray) const override;
private:
	static float reflectance(float cosine, float ref_idx);
private:
	float m_fIor{ 1.2f };
};