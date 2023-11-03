#include "resource_manager.h"

float wrap_value(float n, EWrapMode wrap_mode)
{
	float wrapped{ n };

	switch (wrap_mode)
	{
	case EWrapMode::eRepeat: wrapped = n - std::floor(n); break;
	case EWrapMode::eClampToEdge: wrapped = glm::clamp(n, 0.f, 1.f); break;
	case EWrapMode::eMirroredRepeat:
	{
		float factor = 2.0f * std::floor(std::abs(n));
		bool is_even = ((int)factor % 2) == 0;
		wrapped = is_even ? n - std::floor(n) : 1.0f - (n - std::floor(n));
	} break;
	}
	return wrapped;
}

void CSampler::create(EFilterMode filterMin, EFilterMode filterMag, EWrapMode wrapS, EWrapMode wrapT)
{
	m_minFilter = filterMin;
	m_magFilter = filterMag;
	m_wrapS = wrapS;
	m_wrapT = wrapT;
}

glm::vec2 CSampler::wrap(const glm::vec2& uv)
{
	glm::vec2 wrapped{ 0.f };
	wrapped.x = wrap_value(uv.x, m_wrapS);
	wrapped.y = wrap_value(uv.y, m_wrapT);
	return wrapped;
}

glm::vec4 CSampler::interpolate(const glm::vec4* data, const glm::vec2& coord, float width, float height)
{
	glm::vec4 interpolated_color{ 0.f };

	auto screenspace_uv = glm::vec2(coord.x * (width - 1.f), coord.y * (height - 1.f));

	if (m_minFilter == EFilterMode::eNearest || m_minFilter == EFilterMode::eNearestMipmapLinear || m_minFilter == EFilterMode::eNearestMipmapNearest)
		interpolated_color = nearest_interpolation(data, screenspace_uv, width, height);
	else if(m_minFilter == EFilterMode::eLinear || m_minFilter == EFilterMode::eLinearMipmapLinear || m_minFilter == EFilterMode::eLinearMipmapNearest)
		interpolated_color = bilinear_interpolation(data, screenspace_uv, width, height);

	return interpolated_color;
}

glm::vec4 CSampler::nearest_interpolation(const glm::vec4* data, const glm::vec2& coord, float width, float height)
{
	uint32_t px = static_cast<uint32_t>(coord.x);
	uint32_t py = static_cast<uint32_t>(coord.y);

	return data[px + py * static_cast<uint32_t>(width)];
}

// Based on https://fastcpp.blogspot.com/2011/06/bilinear-pixel-interpolation-using-sse.html
glm::vec4 CSampler::bilinear_interpolation(const glm::vec4* data, const glm::vec2& coord, float width, float height)
{
	int32_t px0 = static_cast<int32_t>(coord.x);
	int32_t py0 = static_cast<int32_t>(coord.y);
	int32_t px1 = std::min(px0 + 1, static_cast<int32_t>(width - 1.f));
	int32_t py1 = std::min(py0 + 1, static_cast<int32_t>(height - 1.f));

	float fx = coord.x - px0;
	float fy = coord.y - py0;

	auto stride = static_cast<int32_t>(width);
	const auto& p0 = data[px0 + py0 * stride];
	const auto& p1 = data[px1 + py0 * stride];
	const auto& p2 = data[px0 + py1 * stride];
	const auto& p3 = data[px1 + py1 * stride];

	auto topMix = p0 + fx * (p1 - p0);
	auto bottomMix = p2 + fx * (p3 - p2);

	return topMix + fy * (bottomMix - topMix);
}