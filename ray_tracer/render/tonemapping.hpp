#pragma once

inline glm::vec3 convertXYZ2Yxy(const glm::vec3& _xyz)
{
	// Reference(s):
	// - XYZ to xyY
	//   https://web.archive.org/web/20191027010144/http://www.brucelindbloom.com/index.html?Eqn_XYZ_to_xyY.html
	float inv = 1.f / glm::dot(_xyz, glm::vec3(1.f));
	return glm::vec3(_xyz.y, _xyz.x * inv, _xyz.y * inv);
}

inline glm::vec3 convertXYZ2RGB(const glm::vec3& _xyz)
{
	glm::vec3 rgb;
	rgb.x = glm::dot(glm::vec3(3.2404542f, -1.5371385f, -0.4985314f), _xyz);
	rgb.y = glm::dot(glm::vec3(-0.9692660f, 1.8760108f, 0.0415560f), _xyz);
	rgb.z = glm::dot(glm::vec3(0.0556434f, -0.2040259f, 1.0572252f), _xyz);
	return rgb;
}

inline glm::vec3 convertRGB2XYZ(const glm::vec3& _rgb)
{
	// Reference(s):
	// - RGB/XYZ Matrices
	//   https://web.archive.org/web/20191027010220/http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
	glm::vec3 xyz;
	xyz.x = glm::dot(glm::vec3(0.4124564f, 0.3575761f, 0.1804375f), _rgb);
	xyz.y = glm::dot(glm::vec3(0.2126729f, 0.7151522f, 0.0721750f), _rgb);
	xyz.z = glm::dot(glm::vec3(0.0193339f, 0.1191920f, 0.9503041f), _rgb);
	return xyz;
}

inline glm::vec3 convertYxy2XYZ(const glm::vec3& _Yxy)
{
	// Reference(s):
	// - xyY to XYZ
	//   https://web.archive.org/web/20191027010036/http://www.brucelindbloom.com/index.html?Eqn_xyY_to_XYZ.html
	glm::vec3 xyz;
	xyz.x = _Yxy.x * _Yxy.y / _Yxy.z;
	xyz.y = _Yxy.x;
	xyz.z = _Yxy.x * (1.f - _Yxy.y - _Yxy.z) / _Yxy.z;
	return xyz;
}

inline glm::vec3 convertRGB2Yxy(const glm::vec3& _rgb)
{
	return convertXYZ2Yxy(convertRGB2XYZ(_rgb));
}

inline glm::vec3 convertYxy2RGB(const glm::vec3& _Yxy)
{
	return convertXYZ2RGB(convertYxy2XYZ(_Yxy));
}

inline glm::vec3 apply_gamma(const glm::vec3& _rgb, float gamma)
{
	return glm::pow(glm::abs(_rgb), glm::vec3(1.f / gamma));
}

inline glm::vec3 Uncharted2Tonemap(const glm::vec3& color)
{
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	float W = 11.2f;
	return ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
}

inline glm::vec3 ACESTonemap(const glm::vec3& color)
{
	constexpr const glm::vec3 A0(0.59719f, 0.35458f, 0.04823f);
	constexpr const glm::vec3 B0(0.07600f, 0.90834f, 0.01566f);
	constexpr const glm::vec3 C0(0.02840f, 0.13383f, 0.83777f);

	auto result = glm::vec3(glm::dot(A0, color), glm::dot(B0, color), glm::dot(C0, color));

	const glm::vec3 a = result * (result + 0.0245786f) - 0.000090537f;
	const glm::vec3 b = result * (0.983729f * result + 0.4329510f) + 0.238081f;
	result = a / b;

	constexpr const glm::vec3 A1(1.60475f, -0.53108f, -0.07367f);
	constexpr const glm::vec3 B1(-0.10208f, 1.10813f, -0.00605f);
	constexpr const glm::vec3 C1(-0.00327f, -0.07276f, 1.07602f);

	result = glm::vec3(glm::dot(A1, result), glm::dot(B1, result), glm::dot(C1, result));

	return glm::clamp(result, 0.f, 1.f);
}