#include "renderer.h"
#include "resources/resource_manager.h"

#include "ecs/components/camera_component.h"

#include "util.h"

#include "resources/bxdf.hpp"

float power_heuristic(int32_t nf, float pdfF, int32_t ng, float pdfG)
{
	float f = static_cast<float>(nf) * pdfF;
	float g = static_cast<float>(ng) * pdfG;
	float f2 = f * f;
	return f2 / (f2 + g * g);
}

glm::vec3 ray_color(const FRay& ray)
{
	auto direction = glm::normalize(ray.m_direction);
	auto t = 0.5f * (direction.y + 1.0f);
	//return glm::vec3(glm::vec3(1.f, 1.f, 1.f) * (1.f - t) + glm::vec3(0.5f, 0.7f, 1.f) * t); // day
	return glm::vec3(glm::vec3(0.01f, 0.01f, 0.03f) * (1.f - t) + glm::vec3(0.0f, 0.0f, 0.0f) * t); // night
}

CRenderCore::CRenderCore(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CRenderCore::create(uint32_t width, uint32_t heigth, uint32_t samples, uint32_t bounces)
{
	// Creating a framebuffer
	m_pFramebuffer = std::make_unique<CFramebuffer>(m_pResourceManager);
	m_pFramebuffer->create(width, heigth);

	m_sampleCount = samples;
	m_bounceCount = bounces;

	m_pixelIterator.resize(width * heigth);
	std::iota(m_pixelIterator.begin(), m_pixelIterator.end(), 0u);
}

void CRenderCore::trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin)
{
	//std::execution::par, 
	std::for_each(std::execution::par, m_pixelIterator.begin(), m_pixelIterator.end(),
		[this, scene, camera, &origin](uint32_t index)
		{
			trace_ray(scene, camera, origin, index);
		});
}

void CRenderCore::trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin, uint32_t ray_index)
{
	auto& viewport_extent = camera->m_viewportExtent;

	auto x = ray_index % viewport_extent.x;
	auto y = ray_index / viewport_extent.x;

	auto& ray_direction = camera->m_vRayDirections[ray_index];

	glm::vec4 final_color{ 0.f };
	for (uint32_t sample = 1u; sample <= m_sampleCount; ++sample)
	{
		FRay ray{};
		ray.m_origin = origin;
		ray.set_direction(ray_direction); //  + glm::vec3(rnd::sample(), rnd::sample(), rnd::sample())
		final_color += glm::vec4(integrate(scene, ray, m_bounceCount), 1.f);
	}

	m_pFramebuffer->add_pixel(x, y, final_color / static_cast<float>(m_sampleCount));
}

glm::vec3 CRenderCore::hit_pixel(CScene* scene, FRay ray, int32_t bounces, glm::vec3 throughput)
{
	//if (bounces <= 0)
	//	return glm::vec3(0.f);
	//
	//FHitResult hit_result{};
	//if (!scene->trace_ray(ray, 0.001f, std::numeric_limits<float>::infinity(), hit_result))
	//	return ray_color(ray);
	//
	//auto& material = m_pResourceManager->get_material(hit_result.m_material_id);
	//
	//float alpha{ 1.f };
	//glm::vec3 brdf_multiplier{};
	//glm::vec3 emitted = material->emit(hit_result);
	//
	//float rr_pdf = glm::max(throughput.x, glm::max(throughput.y, throughput.z));
	//
	//FRay scattered{};
	//if (!material->scatter(ray, hit_result, brdf_multiplier, scattered, alpha) || math::greater_equal_float(rnd::sample(), rr_pdf))
	//	return emitted;
	//
	//throughput *= brdf_multiplier / rr_pdf;
	//
	////if(!math::compare_float(alpha, 1.f))
	////	return emitted + hit_pixel(scene, scattered, bounces - 1, throughput);
	//
	//return emitted + brdf_multiplier * hit_pixel(scene, scattered, bounces - 1, throughput) / rr_pdf;
	return glm::vec3(0.f);
}

glm::vec3 CRenderCore::integrate(CScene* scene, FRay ray, int32_t bounces)
{
	glm::vec3 throughput{ 1.f };
	glm::vec3 color{ 0.f };

	FHitResult hit_result{};
	auto hit_something = scene->trace_ray(ray, 0.001f, std::numeric_limits<float>::infinity(), hit_result);

	for (uint32_t depth = 0u; depth <= bounces; ++depth)
	{
		if (!hit_something)
		{
			color += throughput * ray_color(ray);
			break;
		}

		auto& material = m_pResourceManager->get_material(hit_result.m_material_id);

		if (material->can_emit_light() && depth == 0u)
		{
			color += throughput * material->emit(hit_result);

			if(!material->can_scatter_light())
				break;
		}

		glm::vec3 color = material->sample_diffuse_color(hit_result);
		glm::vec2 mr = material->sample_surface_metallic_roughness(hit_result);
		glm::vec3 normal = material->sample_surface_normal(hit_result);
		COrthonormalBasis basis(normal);

		if (math::isnan(normal))
			int iiiii = 0;

		glm::vec3 wo = basis.to_local(glm::normalize(-ray.m_direction));

		float rr_sample = rnd::sample();
		float light_index_sample = rnd::sample();
		glm::vec2 light_sample = rnd::sample_vec2();
		glm::vec2 bsdf_sample = rnd::sample_vec2();

		size_t light_index = scene->get_light_index(light_index_sample);
		auto& light = scene->get_light(light_index);
		float light_probability = scene->get_light_probability();

		float light_pdf{};
		glm::vec3 light_dir = light.sample(hit_result.m_position, light_sample.x, light_sample.y, light_pdf);
		glm::vec3 wi = basis.to_local(light_dir);

		float cosThetaI = glm::abs(cos_theta(wi));
		if (cosThetaI > 0.f && light_pdf > 0.f)
		{
			FRay light_ray;
			light_ray.m_origin = hit_result.m_position + math::sign(cos_theta(wi)) * normal * 0.001f;
			light_ray.set_direction(light_dir);

			FHitResult light_hit_result{};
			bool light_hit_something = scene->trace_ray(light_ray, 0.001f, std::numeric_limits<float>::infinity(), light_hit_result);
			if (light_hit_something && light_index == light_hit_result.m_primitive_id && light_hit_result.m_primitive_id != hit_result.m_primitive_id)
			{
				float bsdf_pdf = material->pdf(wi, wo, color, mr.x, mr.y, 1.f);
				if (bsdf_pdf > 0.f)
				{
					auto& light_material = m_pResourceManager->get_material(light.get_material_id());
					float mis_weight = power_heuristic(1, light_pdf, 1, bsdf_pdf);
					glm::vec3 bsdf = material->eval(wi, wo, color, mr.x, mr.y, 0.f);
					color += throughput * light_material->emit(light_hit_result) * bsdf * cosThetaI * mis_weight / (light_pdf * light_probability);
					assert(math::isfinite(color) && math::greater_equal_float(color.r, 0.0f) && math::greater_equal_float(color.g, 0.0f) && math::greater_equal_float(color.b, 0.0f));
				}
			}
		}

		float bsdf_pdf{};
		wi = material->sample(wo, color, mr.x, mr.y, 0.f, bsdf_sample.x, bsdf_sample.y, bsdf_pdf);
		cosThetaI = glm::abs(cos_theta(wi));
		if (math::less_equal_float(cosThetaI, 0.f) || math::less_equal_float(bsdf_pdf, 0.f))
			break;

		glm::vec3 bsdf = material->eval(wi, wo, color, mr.x, mr.y, 0.f);
		//if (material->can_emit_light())
		//	bsdf += material->emit(hit_result);

		if (bsdf.x == 0.f && bsdf.y == 0.f && bsdf.z == 0.f)
			int iiiii = 0;

		FRay light_ray{};
		light_ray.m_origin = hit_result.m_position + math::sign(cos_theta(wi)) * normal * 0.001f;
		light_ray.set_direction(basis.to_world(wi));

		FHitResult light_hit_result{};
		bool light_hit_something = scene->trace_ray(light_ray, 0.001f, std::numeric_limits<float>::infinity(), light_hit_result);
		if (light_hit_something && light_index == light_hit_result.m_primitive_id && light_hit_result.m_primitive_id != hit_result.m_primitive_id)
		{
			light_pdf = light.pdf(hit_result.m_position, light_ray.m_direction);
			if (light_pdf > 0.f)
			{
				auto& light_material = m_pResourceManager->get_material(light.get_material_id());
				float mis_weight = power_heuristic(1, bsdf_pdf, 1, light_pdf);
				color += throughput * light_material->emit(light_hit_result) * bsdf * cosThetaI * mis_weight / (bsdf_pdf * light_probability);
				assert(math::isfinite(color) && math::greater_equal_float(color.r, 0.0f) && math::greater_equal_float(color.g, 0.0f) && math::greater_equal_float(color.b, 0.0f));
			}
		}

		throughput *= bsdf * cosThetaI / bsdf_pdf;
		assert(math::isfinite(throughput) && math::greater_equal_float(throughput.r, 0.0f) && math::greater_equal_float(throughput.g, 0.0f) && math::greater_equal_float(throughput.b, 0.0f));

		float rr_prob = glm::min(0.95f, math::max_component(throughput));
		//assert(rr_prob > 0.0f && std::isfinite(rr_prob));

		if (depth >= 3)
		{
			if (rr_sample > rr_prob)
				break;

			throughput /= rr_prob;
		}

		ray = light_ray;
		hit_result = light_hit_result;
		hit_something = light_hit_something;
	}

	return color;
}

const std::unique_ptr<CFramebuffer>& CRenderCore::get_framebuffer() const
{
	return m_pFramebuffer;
}

const std::vector<uint32_t>& CRenderCore::get_pixel_iterator() const
{
	return m_pixelIterator;
}