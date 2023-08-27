#include <benchmark/benchmark.h>

#include "ray_tracer/engine.h"
#include "ray_tracer/math/math.hpp"
#include "util.h"

// Add setting save every N samples
constexpr const uint32_t screen_width = 1280u;
constexpr const uint32_t screen_height = 720u;
constexpr const uint32_t sample_count = 256u;
constexpr const uint32_t bounce_count = 10u;

static void BM_ray_trace(benchmark::State& state)
{
	auto engine = std::make_unique<CRayEngine>();
	//engine->create("../../meshes/Triangle/glTF/Triangle.gltf", screen_width, screen_height, bounce_count);
	engine->create("../../meshes/DamagedHelmet/glTF/DamagedHelmet.gltf", screen_width, screen_height, sample_count, bounce_count);
	//engine->create("../../meshes/CornelBox/Cycles.gltf", screen_width, screen_height, sample_count, bounce_count);
	//engine->create("../../meshes/Duck/Duck.gltf", screen_width, screen_height, sample_count, bounce_count);
	//engine->create("../../meshes/cyberpunk/scene.gltf", screen_width, screen_height, sample_count, bounce_count);
	
	for (auto _ : state)
		engine->update();

	auto& resource_manager = engine->get_resource_manager();
	auto& renderer = engine->get_renderer();
	auto& framebuffer = renderer->get_framebuffer();
	framebuffer->present();
	auto image_id = framebuffer->get_image();

	auto& image = resource_manager->get_image(image_id);

	image->save("final.png");
}
BENCHMARK(BM_ray_trace)->Iterations(1);

BENCHMARK_MAIN();