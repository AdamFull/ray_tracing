#include <benchmark/benchmark.h>

#include "ray_tracer/engine.h"

constexpr const uint32_t screen_width = 1280u;
constexpr const uint32_t screen_height = 720u;

static void BM_ray_trace(benchmark::State& state)
{
	auto engine = std::make_unique<CRayEngine>();
	//engine->create("../../meshes/Triangle/glTF/Triangle.gltf", screen_width, screen_height);
	engine->create("../../meshes/DamagedHelmet/glTF/DamagedHelmet.gltf", screen_width, screen_height);
	
	for (auto _ : state)
		engine->update();

	auto& resource_manager = engine->get_resource_manager();
	auto& renderer = engine->get_renderer();
	auto& framebuffer = renderer->get_framebuffer();
	auto image_id = framebuffer->get_image();

	auto& image = resource_manager->get_image(image_id);

	image->save("outimg.png");
}
BENCHMARK(BM_ray_trace)->Iterations(1);

BENCHMARK_MAIN();