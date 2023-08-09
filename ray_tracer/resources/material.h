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

	glm::vec4 m_baseColorFactor{ 1.f };

	glm::vec3 m_emissiveFactor{ 1.f };
	float m_emissiveStrength{ 1.f };

	float m_fRoughnessFactor{ 1.f };
	float m_fMetallicFactor{ 1.f };
};

class CResourceManager;

class CMaterial
{
public:
	CMaterial() = default;
	virtual ~CMaterial() {}

	virtual glm::vec3 emit(const FHitResult& hit_result) const { return glm::vec3(0.f); }

	virtual void create(const FMaterialCreateInfo& createInfo) = 0;
	virtual bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf) const { return false; }
	virtual float scatter_pdf(const FRay& in_ray, const FHitResult& hit_result, const FRay& out_ray) const { return 0.f; };
protected:
	glm::vec4 sample_texture(ETextureType texture, const glm::vec2& uv) const;
	glm::vec3 sample_tangent_space_normal(const glm::vec2& uv, const glm::vec3& tangent, const glm::vec3& bitangent, const glm::vec3& normal) const;
protected:
	std::unordered_map<ETextureType, resource_id_t> m_textures{};
	EAlphaMode m_alphaMode{};
	float m_alphaCutoff{};
	CResourceManager* m_pResourceManager{ nullptr };
};

class CLambertianMaterial : public CMaterial
{
public:
	CLambertianMaterial() = default;
	CLambertianMaterial(CResourceManager* resource_manager);
	CLambertianMaterial(const glm::vec3& albedo) : m_albedo(albedo) {}
	~CLambertianMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf) const override;
	//float scatter_pdf(const FRay& in_ray, const FHitResult& hit_result, const FRay& out_ray) const override;
private:
	glm::vec3 m_albedo{};
};

class CMetalRoughnessMaterial : public CMaterial
{
public:
	CMetalRoughnessMaterial() = default;
	CMetalRoughnessMaterial(CResourceManager* resource_manager);
	CMetalRoughnessMaterial(const glm::vec3& albedo) : m_albedo(albedo) {}
	~CMetalRoughnessMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf) const override;
	float scatter_pdf(const FRay& in_ray, const FHitResult& hit_result, const FRay& out_ray) const override;
private:
	
	glm::vec3 m_albedo{};
	float m_metallic{ 1.f };
	float m_roughness{ 1.f };
};

class CEmissiveMaterial : public CMaterial
{
public:
	CEmissiveMaterial() = default;
	CEmissiveMaterial(CResourceManager* resource_manager);
	~CEmissiveMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	glm::vec3 emit(const FHitResult& hit_result) const override;
private:
	glm::vec3 m_emissive{};
	float m_emissionStrength{ 1.f };
};

class CDielectricMaterial : public CMaterial
{
public:
	CDielectricMaterial() = default;
	CDielectricMaterial(float ior) : m_fIor(ior) {}
	~CDielectricMaterial() override = default;

	void create(const FMaterialCreateInfo& createInfo) override;
	bool scatter(const FRay& in_ray, const FHitResult& hit_result, glm::vec3& color, FRay& out_ray, float& pdf) const override;
private:
	static float reflectance(float cosine, float ref_idx);
private:
	float m_fIor{ 1.2f };
};