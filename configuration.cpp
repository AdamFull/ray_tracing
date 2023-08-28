#include "configuration.h"

#include <uparse.hpp>
#include <fstream>

namespace fs = std::filesystem;

void to_json(nlohmann::json& json, const FFramebufferConfig& type)
{
    utl::serialize_to("width", json, type.m_width, type.m_width != 0u);
    utl::serialize_to("height", json, type.m_height, type.m_height != 0u);
    utl::serialize_to("antialiasing", json, type.m_antialiasing, type.m_antialiasing);
}

void from_json(const nlohmann::json& json, FFramebufferConfig& type)
{
    utl::parse_from("width", json, type.m_width);
    utl::parse_from("height", json, type.m_height);
    utl::parse_from("antialiasing", json, type.m_antialiasing);
}


void to_json(nlohmann::json& json, const FOutputConfig& type)
{
	utl::serialize_to("image_name", json, type.m_image_name, !type.m_image_name.empty());
}

void from_json(const nlohmann::json& json, FOutputConfig& type)
{
	utl::parse_from("image_name", json, type.m_image_name);
}


void to_json(nlohmann::json& json, const FIntegratorConfig& type)
{
	utl::serialize_to("sample_count", json, type.m_sample_count, type.m_sample_count > 1u);
	utl::serialize_to("bounce_count", json, type.m_bounce_count, type.m_bounce_count > 3u);
	utl::serialize_to("rr_threshold", json, type.m_rr_threshold, type.m_rr_threshold != 3u);
}

void from_json(const nlohmann::json& json, FIntegratorConfig& type)
{
	utl::parse_from("sample_count", json, type.m_sample_count);
	utl::parse_from("bounce_count", json, type.m_bounce_count);
	utl::parse_from("rr_threshold", json, type.m_rr_threshold);
}


void to_json(nlohmann::json& json, const FTonemapConfig& type)
{
	utl::serialize_to("exposure", json, type.m_exposure, true);
	utl::serialize_to("gamma", json, type.m_gamma, true);
}

void from_json(const nlohmann::json& json, FTonemapConfig& type)
{
	utl::parse_from("exposure", json, type.m_exposure);
	utl::parse_from("gamma", json, type.m_gamma);
}


void to_json(nlohmann::json& json, const FSceneConfig& type)
{
	utl::serialize_to("path", json, type.m_scene_path, true);
}

void from_json(const nlohmann::json& json, FSceneConfig& type)
{
	utl::parse_from("path", json, type.m_scene_path);
}


void to_json(nlohmann::json& json, const FConfiguration& type)
{
	utl::serialize_to("framebuffer", json, type.m_fbcfg, true);
	utl::serialize_to("integrator", json, type.m_icfg, true);
	utl::serialize_to("output", json, type.m_ocfg, true);
	utl::serialize_to("scene", json, type.m_scfg, true);
	utl::serialize_to("tonemapping", json, type.m_tmcfg, true);
}

void from_json(const nlohmann::json& json, FConfiguration& type)
{
	utl::parse_from("framebuffer", json, type.m_fbcfg, true);
	utl::parse_from("integrator", json, type.m_icfg, true);
	utl::parse_from("output", json, type.m_ocfg, true);
	utl::parse_from("scene", json, type.m_scfg, true);
	utl::parse_from("tonemapping", json, type.m_tmcfg, true);
}

void CConfiguration::load(const std::string& path)
{
	auto cfg_path = fs::path(path);

	if (!fs::exists(path))
		return;

	std::ifstream cfg_file(cfg_path, std::ios_base::in || std::ios_base::binary);
	if (!cfg_file.is_open())
		return;

	std::vector<uint8_t> cfg_data((std::istreambuf_iterator<char>(cfg_file)), std::istreambuf_iterator<char>());
	if (cfg_data.empty())
		return;

	auto cfg_json = nlohmann::json::parse(cfg_data);
	cfg_json.get_to(m_config);
}

const FConfiguration& CConfiguration::get() const
{
	return m_config;
}

FConfiguration& CConfiguration::get()
{
	return m_config;
}