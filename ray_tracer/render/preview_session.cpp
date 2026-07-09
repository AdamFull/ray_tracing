#include "preview_session.h"
#include "preview_window.h"

#include "engine.h"
#include "integrator.h"
#include "framebuffer.h"
#include "tonemapping.hpp"

#include "scene.h"
#include "resources/resource_manager.h"

#include "ecs/systems/camera_system.h"
#include "ecs/components/camera_component.h"
#include "ecs/components/transform_component.h"
#include "ecs/components/hierarchy_component.h"

#include <configuration.h>
#include <logger/logger.h>

#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <thread>
#include <vector>

namespace
{
	glm::vec3 linear_to_srgb(const glm::vec3& c)
	{
		glm::vec3 lo = c * 12.92f;
		glm::vec3 hi = 1.055f * glm::pow(glm::max(c, glm::vec3(0.f)), glm::vec3(1.f / 2.4f)) - 0.055f;
		glm::bvec3 cutoff = glm::lessThan(c, glm::vec3(0.0031308f));
		return glm::mix(hi, lo, cutoff);
	}

	uint32_t pack_bgrx(const glm::vec3& linear_color)
	{
		glm::vec3 srgb = linear_to_srgb(ACESTonemap(linear_color));
		auto to8 = [](float v) { return static_cast<uint32_t>(glm::clamp(v, 0.f, 1.f) * 255.f); };
		return (to8(srgb.r) << 16) | (to8(srgb.g) << 8) | to8(srgb.b);
	}

	// Locate the camera the configuration selects (by name, else by index), mirroring the
	// selection done by the render system so preview and full render use the same camera.
	bool find_selected_camera(entt::registry& registry, FCameraComponent*& camera, FTransformComponent*& transform)
	{
		auto& config = CConfiguration::getInstance()->get();

		if (!config.m_scfg.m_camera_name.empty())
		{
			auto view = registry.view<FTransformComponent, FHierarchyComponent, FCameraComponent>();
			for (auto [entity, t, h, c] : view.each())
			{
				if (config.m_scfg.m_camera_name == h.m_name)
				{
					camera = &c;
					transform = &t;
					return true;
				}
			}
			return false;
		}

		size_t camera_idx{ 0ull };
		auto view = registry.view<FTransformComponent, FCameraComponent>();
		for (auto [entity, t, c] : view.each())
		{
			if (config.m_scfg.m_camera_id == camera_idx)
			{
				camera = &c;
				transform = &t;
				return true;
			}
			++camera_idx;
		}
		return false;
	}
}

void run_preview(CRayEngine* engine)
{
	auto& config = CConfiguration::getInstance()->get();
	auto& scene = engine->get_scene();
	auto& full_renderer = engine->get_renderer();
	auto& resource_manager = engine->get_resource_manager();
	auto& registry = scene->get_registry();

	FCameraComponent* camera{ nullptr };
	FTransformComponent* transform{ nullptr };
	if (!find_selected_camera(registry, camera, transform))
	{
		log_error("Preview: no camera available in the scene.");
		return;
	}

	// Scene scale drives the fly-camera movement speed.
	auto bounds = scene->get_bounds();
	float scene_radius = 0.5f * glm::length(bounds.extent());
	if (!(scene_radius > 0.f))
		scene_radius = 1.f;

	// Preview render resolution (kept low for interactivity); the window shows it upscaled.
	const uint32_t preview_height = 320u;
	const uint32_t preview_width = static_cast<uint32_t>(camera->m_aspect * preview_height);
	const glm::uvec2 preview_extent{ preview_width, preview_height };
	const uint32_t preview_bounces = std::min(config.m_icfg.m_bounce_count, 6u);

	CIntegrator preview_renderer(resource_manager.get());
	preview_renderer.create(preview_width, preview_height, 1u);

	CPreviewWindow window;
	const int window_scale = 2;
	if (!window.create(static_cast<int>(preview_width) * window_scale, static_cast<int>(preview_height) * window_scale, "Path Tracer - Preview"))
	{
		log_error("Preview: failed to create the window.");
		return;
	}

	CCameraSystem camera_system;

	// Initialise the fly-camera from the selected camera's current pose.
	glm::vec3 position = transform->m_position_g;
	glm::vec3 forward = glm::normalize(transform->m_rotation_g * glm::vec3(0.f, 0.f, -1.f));
	// With orientation = angleAxis(yaw, +Y) * angleAxis(pitch, +X), the forward vector is
	// (-cos(pitch)*sin(yaw), sin(pitch), -cos(pitch)*cos(yaw)); invert to recover yaw/pitch.
	float yaw = std::atan2(-forward.x, -forward.z);
	float pitch = std::asin(glm::clamp(forward.y, -1.f, 1.f));
	float fov = camera->m_fov;

	std::vector<uint32_t> display(static_cast<size_t>(preview_width) * preview_height);

	uint32_t frame_index = 0u;
	bool need_reset = true;

	const float look_sensitivity = 0.0035f;
	const float pitch_limit = glm::radians(89.f);

	log_info("Preview started. Right-drag to look, WASD/QE to move, wheel = fov, P = full render, Esc = quit.");

	auto previous_time = std::chrono::steady_clock::now();

	while (window.poll())
	{
		auto current_time = std::chrono::steady_clock::now();
		float dt = std::chrono::duration<float>(current_time - previous_time).count();
		previous_time = current_time;
		dt = glm::clamp(dt, 0.f, 0.1f);

		if (window.key_pressed(preview_key::escape))
			break;

		bool changed = false;

		// Look (only while the right mouse button is held).
		if (window.right_mouse_down())
		{
			float dx{ 0.f }, dy{ 0.f };
			window.consume_mouse_delta(dx, dy);
			if (dx != 0.f || dy != 0.f)
			{
				// Drag right -> turn right: forward.x grows with -sin(yaw), so yaw decreases.
				yaw -= dx * look_sensitivity;
				pitch -= dy * look_sensitivity;
				pitch = glm::clamp(pitch, -pitch_limit, pitch_limit);
				changed = true;
			}
		}

		// Field of view.
		float scroll = window.consume_scroll();
		if (scroll != 0.f)
		{
			fov = glm::clamp(fov - scroll * 3.f, 5.f, 120.f);
			changed = true;
		}

		// Orientation basis for this frame.
		glm::quat orientation = glm::angleAxis(yaw, glm::vec3(0.f, 1.f, 0.f)) * glm::angleAxis(pitch, glm::vec3(1.f, 0.f, 0.f));
		glm::vec3 fwd = orientation * glm::vec3(0.f, 0.f, -1.f);
		glm::vec3 right = glm::normalize(glm::cross(fwd, glm::vec3(0.f, 1.f, 0.f)));
		glm::vec3 world_up{ 0.f, 1.f, 0.f };

		// Movement.
		glm::vec3 move{ 0.f };
		if (window.key_down('W')) move += fwd;
		if (window.key_down('S')) move -= fwd;
		if (window.key_down('D')) move += right;
		if (window.key_down('A')) move -= right;
		if (window.key_down('E') || window.key_down(preview_key::space)) move += world_up;
		if (window.key_down('Q') || window.key_down(preview_key::control)) move -= world_up;

		if (glm::dot(move, move) > 0.f)
		{
			float speed = scene_radius * (window.key_down(preview_key::shift) ? 3.f : 1.f);
			position += glm::normalize(move) * speed * dt;
			changed = true;
		}

		if (changed)
		{
			transform->m_position = position;
			transform->m_position_g = position;
			transform->m_rotation = orientation;
			transform->m_rotation_g = orientation;
			camera->m_fov = fov;
			camera->m_bWasMoved = true;
			// Force projection + ray-direction recompute (covers fov changes too).
			camera->m_viewportExtent = glm::uvec2(0u);
			need_reset = true;
		}

		if (need_reset)
		{
			camera_system.update_camera(registry, preview_extent, camera, transform);
			preview_renderer.get_framebuffer()->clear(glm::vec3(0.f), FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT);
			frame_index = 0u;
			need_reset = false;
		}

		// Accumulate one more sample and resolve the current average to the window.
		preview_renderer.render_preview(scene.get(), camera, transform->m_position_g, frame_index, preview_bounces);
		++frame_index;

		auto& framebuffer = preview_renderer.get_framebuffer();
		float inv = 1.f / static_cast<float>(frame_index);
		for (uint32_t y = 0u; y < preview_height; ++y)
		{
			for (uint32_t x = 0u; x < preview_width; ++x)
			{
				glm::vec3 color = framebuffer->get_pixel(x, y, FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT) * inv;
				display[static_cast<size_t>(preview_height - 1u - y) * preview_width + x] = pack_bgrx(color);
			}
		}
		window.present(display.data(), static_cast<int>(preview_width), static_cast<int>(preview_height));

		{
			char title[160];
			std::snprintf(title, sizeof(title), "Path Tracer - Preview | %u spp | fov %.0f | P: render  Esc: quit", frame_index, fov);
			window.set_title(title);
		}

		// Full-quality render on demand.
		if (window.key_pressed('P'))
		{
			log_info("Starting full-quality render ({} spp)...", config.m_icfg.m_sample_count);

			auto full_extent = full_renderer->get_framebuffer()->get_extent();

			camera->m_bWasMoved = true;
			camera->m_viewportExtent = glm::uvec2(0u);
			camera_system.update_camera(registry, full_extent, camera, transform);

			auto& framebuffer = full_renderer->get_framebuffer();
			// clear() does a direct per-attachment lookup, so clear each attachment on its own
			// (passing the combined ALL mask would insert a null buffer and crash).
			framebuffer->clear(glm::vec3(0.f), FRAMEBUFFER_COLOR_ATTACHMENT_FLAG_BIT);
			framebuffer->clear(glm::vec3(0.f), FRAMEBUFFER_ALBEDO_ATTACHMENT_FLAG_BIT);
			framebuffer->clear(glm::vec3(0.f), FRAMEBUFFER_NORMAL_ATTACHMENT_FLAG_BIT);

			// Run the full trace on a worker thread so the window keeps pumping messages and
			// stays responsive (and reliably reaches the save step) instead of freezing.
			std::atomic<bool> render_done{ false };
			std::thread worker([&]()
			{
				full_renderer->trace_ray(scene.get(), camera, transform->m_position_g);
				render_done.store(true, std::memory_order_release);
			});

			auto render_start = std::chrono::steady_clock::now();
			bool window_open = true;
			while (!render_done.load(std::memory_order_acquire))
			{
				window_open = window.poll();
				float elapsed = std::chrono::duration<float>(std::chrono::steady_clock::now() - render_start).count();
				char t[192];
				std::snprintf(t, sizeof(t), "Path Tracer - Rendering full quality... %u spp | %.1fs (please wait)", config.m_icfg.m_sample_count, elapsed);
				window.set_title(t);
				std::this_thread::sleep_for(std::chrono::milliseconds(50));
			}
			worker.join();

			framebuffer->present();
			auto& image = resource_manager->get_image(framebuffer->get_image());
			image->save(config.m_ocfg.m_image_name);
			log_info("Full render saved to {}.", config.m_ocfg.m_image_name);

			if (!window_open)
				break;

			// Restore the preview camera rays and restart accumulation.
			camera->m_bWasMoved = true;
			camera->m_viewportExtent = glm::uvec2(0u);
			previous_time = std::chrono::steady_clock::now();
			need_reset = true;
		}
	}

	log_info("Preview closed.");
}
