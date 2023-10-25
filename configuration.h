#pragma once

#include <upattern.hpp>

struct FFramebufferConfig
{
	uint32_t m_width{ 1280u };
	uint32_t m_height{ 720u };
	float m_aspect_ratio{ 1.777777779f };
	std::string m_aspect_mode{ "config" }; // camera - use aspect from camera, config - use aspect from config, none - calculate aspect from resolution
	bool m_antialiasing{ false };
};

struct FOutputConfig
{
	std::string m_image_name{ "final.png" };
};

struct FIntegratorConfig
{
	uint32_t m_sample_count{ 5u };
	uint32_t m_bounce_count{ 15u };
	uint32_t m_rr_threshold{ 3u };

	// Estimator settings
	bool m_use_estimator{ false };
	float m_estimator_tolerance{ 0.05f };
};

struct FTonemapConfig
{
	float m_gamma{ 2.2f };
	float m_exposure{ 4.f };
};

struct FSkyboxConfig
{
	struct FGradient {
		glm::vec3 m_begin;
		glm::vec3 m_end;
	} m_gradient;
	glm::vec3 m_solid_color;
};

struct FSceneConfig
{
	std::string m_scene_path{};
	std::string m_camera_name{};
	size_t m_camera_id{ 1ull };
	FSkyboxConfig m_skybox{};
};

struct FConfiguration
{
	FFramebufferConfig m_fbcfg{};
	FOutputConfig m_ocfg{};
	FIntegratorConfig m_icfg{};
	FTonemapConfig m_tmcfg{};
	FSceneConfig m_scfg{};
};

class CConfiguration : public utl::singleton<CConfiguration>
{
public:
	void load(const std::string& path);

	const FConfiguration& get() const;
	FConfiguration& get();
private:
	FConfiguration m_config{};
};