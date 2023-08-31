#include "configuration.h"

#include <uparse.hpp>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

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

	utl::serialize_to("use_estimator", json, type.m_use_estimator, type.m_use_estimator);
	utl::serialize_to("estimator_tolerance", json, type.m_estimator_tolerance, true);
}

void from_json(const nlohmann::json& json, FIntegratorConfig& type)
{
	utl::parse_from("sample_count", json, type.m_sample_count);
	utl::parse_from("bounce_count", json, type.m_bounce_count);
	utl::parse_from("rr_threshold", json, type.m_rr_threshold);

	utl::parse_from("use_estimator", json, type.m_use_estimator);
	utl::parse_from("estimator_tolerance", json, type.m_estimator_tolerance);
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


void to_json(nlohmann::json& json, const FSkyboxConfig::FGradient& type)
{
	float tmp[3ull];
	memcpy(tmp, glm::value_ptr(type.m_begin), 3ull * sizeof(float));
	json["begin"] = tmp;

	memcpy(tmp, glm::value_ptr(type.m_end), 3ull * sizeof(float));
	json["end"] = tmp;
}

void from_json(const nlohmann::json& json, FSkyboxConfig::FGradient& type)
{
	float tmp[3ull];
	if (auto obj = json.find("begin"); obj != json.end())
	{
		obj->get_to(tmp);
		type.m_begin = glm::make_vec3(tmp);
	}

	if (auto obj = json.find("end"); obj != json.end())
	{
		obj->get_to(tmp);
		type.m_end = glm::make_vec3(tmp);
	}
}


void to_json(nlohmann::json& json, const FSkyboxConfig& type)
{
	utl::serialize_to("gradient", json, type.m_gradient, true);

	float tmp[3ull];
	memcpy(tmp, glm::value_ptr(type.m_solid_color), 3ull * sizeof(float));
	json["solid_color"] = tmp;
}

void from_json(const nlohmann::json& json, FSkyboxConfig& type)
{
	utl::parse_from("gradient", json, type.m_gradient);
	
	float tmp[3ull];
	if (auto obj = json.find("end"); obj != json.end())
	{
		obj->get_to(tmp);
		type.m_solid_color = glm::make_vec3(tmp);
	}
}


void to_json(nlohmann::json& json, const FSceneConfig& type)
{
	utl::serialize_to("path", json, type.m_scene_path, true);
	utl::serialize_to("skybox", json, type.m_skybox, true);
}

void from_json(const nlohmann::json& json, FSceneConfig& type)
{
	utl::parse_from("path", json, type.m_scene_path);
	utl::parse_from("skybox", json, type.m_skybox);
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