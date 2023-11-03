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
	void create(EFilterMode filterMin, EFilterMode filterMag, EWrapMode wrapS, EWrapMode wrapT);

	glm::vec2 wrap(const glm::vec2& uv);
	glm::vec4 interpolate(const glm::vec4* data, const glm::vec2& coord, float width, float height);
private:
	glm::vec4 nearest_interpolation(const glm::vec4* data, const glm::vec2& coord, float width, float height);
	glm::vec4 bilinear_interpolation(const glm::vec4* data, const glm::vec2& coord, float width, float height);
private:
	EFilterMode m_minFilter{};
	EFilterMode m_magFilter{};
	EWrapMode m_wrapS{};
	EWrapMode m_wrapT{};
};