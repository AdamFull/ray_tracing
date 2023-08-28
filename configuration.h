#pragma once

#include <upattern.hpp>

struct FFramebufferConfig
{
	uint32_t m_width{ 1280u };
	uint32_t m_height{ 720u };
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
};

struct FTonemapConfig
{
	float m_gamma{ 2.2f };
	float m_exposure{ 4.f };
};

struct FSceneConfig
{
	std::string m_scene_path{};
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