#include <benchmark/benchmark.h>

#include "ray_tracer/engine.h"
#include "ray_tracer/math/math.hpp"
#include "util.h"

#include "argparser.h"
#include "configuration.h"

#include <logger/logger.h>
#include <utime.hpp>

int main(int argc, char** argv)
{
	log_add_file_output("log.log");
	log_add_cout_output();
	log_init("Path Tracer", "1.0.0");

	utl::stopwatch sw{};
	utl::stopwatch timer{};

	CArgumentParser argparse(argc, argv);
	CConfiguration::getInstance()->load("config.json");

	auto& config = CConfiguration::getInstance()->get();

	config.m_scfg.m_scene_path = argparse.try_get("--in", config.m_scfg.m_scene_path);
	config.m_ocfg.m_image_name = argparse.try_get("--out", config.m_ocfg.m_image_name);
	config.m_fbcfg.m_width = argparse.try_get("--width", config.m_fbcfg.m_width);
	config.m_fbcfg.m_height = argparse.try_get("--height", config.m_fbcfg.m_height);
	config.m_icfg.m_sample_count = argparse.try_get("--samples", config.m_icfg.m_sample_count);
	config.m_icfg.m_bounce_count = argparse.try_get("--bounces", config.m_icfg.m_bounce_count);
	config.m_icfg.m_rr_threshold = argparse.try_get("--rr", config.m_icfg.m_rr_threshold);

	log_info("Configuration loaded by {}s", timer.stop<float>());

	auto engine = std::make_unique<CRayEngine>();
	engine->create();

	log_info("Path tracer initialized by {}s", timer.stop<float>());

	engine->update();

	log_info("Image traced by {}s", timer.stop<float>());

	auto& resource_manager = engine->get_resource_manager();
	auto& renderer = engine->get_renderer();
	auto& framebuffer = renderer->get_framebuffer();
	framebuffer->present();
	auto image_id = framebuffer->get_image();

	auto& image = resource_manager->get_image(image_id);

	image->save(config.m_ocfg.m_image_name);

	log_info("Result saved by {}s.", timer.stop<float>());
	log_info("Running time {}s.", sw.stop<float>());

	return 0;
}