#pragma once

enum class EFilterMode
{
	eNearest = 9728,
	eLinear = 9729,
	eNearestMipmapNearest = 9984,
	eNearestMipmapLinear = 9985,
	eLinearMipmapNearest = 9986,
	eLinearMipmapLinear = 9987
};

enum class EWrapMode
{
	eRepeat = 10497,
	eClampToEdge = 33071,
	eMirroredRepeat = 33648
};

class CImage;
class CResourceManager;

class CSampler
{
public:
	CSampler(CResourceManager* resource_manager);
	void create(EFilterMode filterMin, EFilterMode filterMag, EWrapMode wrapS, EWrapMode wrapT);

	glm::vec4 sample(resource_id_t image_id, const glm::vec2& uv);
protected:
	glm::vec4 nearest_interpolation(CImage* image, uint32_t x, uint32_t y);
	glm::vec4 bilinear_interpolation(CImage* image, uint32_t x, uint32_t y);
private:
	CResourceManager* m_pResourceManager{ nullptr };
	EFilterMode m_minFilter{};
	EFilterMode m_magFilter{};
	EWrapMode m_wrapS{};
	EWrapMode m_wrapT{};
};