#include <benchmark/benchmark.h>

#include "ray_tracer/engine.h"
#include "ray_tracer/math/math.hpp"
#include "util.h"

// Add setting save every N samples
constexpr const uint32_t screen_width = 1280u;
constexpr const uint32_t screen_height = 720u;
constexpr const uint32_t sample_count = 5u;
constexpr const uint32_t bounce_count = 100u;

//static void no_sse_dot(const std::vector<math::vec2<float>>& vecs, float& v0, float& v1, float& v2, float& v3)
//{
//	for (size_t idx = 0ull; idx < 8192ull; idx += 8ull)
//	{
//		v0 = vecs[idx + 0ull].x * vecs[idx + 1ull].x + vecs[idx + 0ull].y * vecs[idx + 1ull].y;
//		v1 = vecs[idx + 2ull].x * vecs[idx + 3ull].x + vecs[idx + 2ull].y * vecs[idx + 3ull].y;
//		v2 = vecs[idx + 4ull].x * vecs[idx + 5ull].x + vecs[idx + 4ull].y * vecs[idx + 5ull].y;
//		v3 = vecs[idx + 6ull].x * vecs[idx + 7ull].x + vecs[idx + 6ull].y * vecs[idx + 7ull].y;
//
//		BENCHMARK_DONT_OPTIMIZE(v0);
//		BENCHMARK_DONT_OPTIMIZE(v1);
//		BENCHMARK_DONT_OPTIMIZE(v2);
//		BENCHMARK_DONT_OPTIMIZE(v3);
//	}
//}
//
//static void BM_no_sse_dot(benchmark::State& state)
//{
//	std::vector<math::vec2<float>> vecs;
//	for (size_t idx = 0ull; idx < 8192ull; ++idx)
//		vecs.emplace_back(math::vec2<float>(random<float>(-1.f, 1.f), random<float>(-1.f, 1.f)));
//
//	float v0, v1, v2, v3;
//
//	for (auto _ : state)
//		no_sse_dot(vecs, v0, v1, v2, v3);
//}
//BENCHMARK(BM_no_sse_dot)->Iterations(1000000);
//
//static void parallel_sse_dot(const std::vector<math::vec2<float>>& vecs, float& v0, float& v1, float& v2, float& v3)
//{
//	for (size_t idx = 0ull; idx < 8192ull; idx += 8ull)
//	{
//		auto res = math::parallel_dot_product<float>(math::make_parallel_batch<float>(vecs.data(), idx));
//
//		v0 = res[0ull];
//		v1 = res[1ull];
//		v2 = res[2ull];
//		v3 = res[3ull];
//
//		BENCHMARK_DONT_OPTIMIZE(v0);
//		BENCHMARK_DONT_OPTIMIZE(v1);
//		BENCHMARK_DONT_OPTIMIZE(v2);
//		BENCHMARK_DONT_OPTIMIZE(v3);
//	}
//}
//
//static void BM_parallel_sse_dot(benchmark::State& state)
//{
//	std::vector<math::vec2<float>> vecs;
//	for (size_t idx = 0ull; idx < 8192ull; ++idx)
//		vecs.emplace_back(math::vec2<float>(random<float>(-1.f, 1.f), random<float>(-1.f, 1.f)));
//
//	float v0, v1, v2, v3;
//
//	for (auto _ : state)
//		parallel_sse_dot(vecs, v0, v1, v2, v3);
//}
//BENCHMARK(BM_parallel_sse_dot)->Iterations(1000000);

//static void no_sse_dot(const std::vector<math::vec2<float>>& vecs, float& d0, float& d1, float& d2, float& d3)
//{
//	for (size_t idx = 0ull; idx < 4096ull; idx += 4ull)
//	{
//		d0 = vecs[idx + 0ull].x * vecs[idx + 1ull].x + vecs[idx + 0ull].y * vecs[idx + 1ull].y;
//		d1 = vecs[idx + 2ull].x * vecs[idx + 3ull].x + vecs[idx + 2ull].y * vecs[idx + 3ull].y;
//	}
//
//	BENCHMARK_DONT_OPTIMIZE(d0);
//	BENCHMARK_DONT_OPTIMIZE(d1);
//	BENCHMARK_DONT_OPTIMIZE(d2);
//	BENCHMARK_DONT_OPTIMIZE(d3);
//}
//
//static void BM_no_sse_dot(benchmark::State& state)
//{
//	std::vector<math::vec2<float>> vecs;
//	for (size_t idx = 0ull; idx < 4096ull; ++idx)
//		vecs.emplace_back(math::vec2<float>(random<float>(-1.f, 1.f), random<float>(-1.f, 1.f)));
//
//	float d0, d1, d2, d3;
//
//	for (auto _ : state)
//		no_sse_dot(vecs, d0, d1, d2, d3);
//
//	BENCHMARK_DONT_OPTIMIZE(d0);
//	BENCHMARK_DONT_OPTIMIZE(d1);
//	BENCHMARK_DONT_OPTIMIZE(d2);
//	BENCHMARK_DONT_OPTIMIZE(d3);
//}
//BENCHMARK(BM_no_sse_dot)->Iterations(1000000);
//
//static void parallel_sse_dot(std::vector<math::vec2<float>>& vecs, float& d0, float& d1, float& d2, float& d3)
//{
//	
//	for (size_t idx = 0ull; idx < 4096ull; idx += 4ull)
//	{
//		auto r1 = math::parallel_dot_product<float>(math::make_parallel_batch(vecs.data(), idx));
//		d0 = r1[0ull];
//		d1 = r1[1ull];
//	}
//
//	BENCHMARK_DONT_OPTIMIZE(d0);
//	BENCHMARK_DONT_OPTIMIZE(d1);
//	BENCHMARK_DONT_OPTIMIZE(d2);
//	BENCHMARK_DONT_OPTIMIZE(d3);
//}
//
//static void BM_parallel_sse_dot(benchmark::State& state)
//{
//	std::vector<math::vec2<float>> vecs;
//	for (size_t idx = 0ull; idx < 4096ull; ++idx)
//		vecs.emplace_back(math::vec2<float>(random<float>(-1.f, 1.f), random<float>(-1.f, 1.f)));
//
//	float d0, d1, d2, d3;
//
//	for (auto _ : state)
//		parallel_sse_dot(vecs, d0, d1, d2, d3);
//
//	BENCHMARK_DONT_OPTIMIZE(d0);
//	BENCHMARK_DONT_OPTIMIZE(d1);
//	BENCHMARK_DONT_OPTIMIZE(d2);
//	BENCHMARK_DONT_OPTIMIZE(d3);
//}
//BENCHMARK(BM_parallel_sse_dot)->Iterations(1000000);

//static void no_sse_normalize(const std::vector<math::vec2<float>>& vecs, math::vec2<float>& u0, math::vec2<float>& u1)
//{
//	for (size_t idx = 0ull; idx < 4096ull; idx += 2ull)
//	{
//		u0 = vecs[idx + 0ull] / std::sqrt(vecs[idx + 0ull].x * vecs[idx + 0ull].x + vecs[idx + 0ull].y * vecs[idx + 0ull].y);
//		u1 = vecs[idx + 1ull] / std::sqrt(vecs[idx + 1ull].x * vecs[idx + 1ull].x + vecs[idx + 1ull].y * vecs[idx + 1ull].y);
//	}
//	
//	BENCHMARK_DONT_OPTIMIZE(u0);
//	BENCHMARK_DONT_OPTIMIZE(u1);
//}
//
//static void BM_no_sse_normalize(benchmark::State& state)
//{
//	std::vector<math::vec2<float>> vecs;
//	for (size_t idx = 0ull; idx < 4096ull; ++idx)
//		vecs.emplace_back(math::vec2<float>(random<float>(-1.f, 1.f), random<float>(-1.f, 1.f)));
//
//	math::vec2<float> u0, u1;
//
//	for (auto _ : state)
//		no_sse_normalize(vecs, u0, u1);
//
//	BENCHMARK_DONT_OPTIMIZE(u0);
//	BENCHMARK_DONT_OPTIMIZE(u1);
//}
//BENCHMARK(BM_no_sse_normalize)->Iterations(1000000);
//
//static void parallel_sse_normalize(const std::vector<math::vec2<float>>& vecs, math::vec2<float>& u0, math::vec2<float>& u1)
//{
//	for (size_t idx = 0ull; idx < 4096ull; idx += 2ull)
//		std::tie(u0, u1) = math::parallel_normalize<float>(vecs[idx + 0ull], vecs[idx + 1ull]);
//	
//	BENCHMARK_DONT_OPTIMIZE(u0);
//	BENCHMARK_DONT_OPTIMIZE(u1);
//}
//
//static void BM_parallel_sse_normalize(benchmark::State& state)
//{
//	std::vector<math::vec2<float>> vecs;
//	for (size_t idx = 0ull; idx < 4096ull; ++idx)
//		vecs.emplace_back(math::vec2<float>(random<float>(-1.f, 1.f), random<float>(-1.f, 1.f)));
//
//	math::vec2<float> u0, u1;
//
//	for (auto _ : state)
//		parallel_sse_normalize(vecs, u0, u1);
//
//	BENCHMARK_DONT_OPTIMIZE(u0);
//	BENCHMARK_DONT_OPTIMIZE(u1);
//}
//BENCHMARK(BM_parallel_sse_normalize)->Iterations(1000000);

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