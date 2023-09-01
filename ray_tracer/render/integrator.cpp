#include "integrator.h"
#include "resources/resource_manager.h"

#include "ecs/components/camera_component.h"

#include "util.h"

#include "resources/bxdf.hpp"

#include <configuration.h>

float balance_heuristic(float pdfF, float pdfG)
{
	float f_sq = pdfF * pdfF;
	float g_sq = pdfG * pdfG;
	return f_sq / (f_sq + g_sq);
}

glm::vec3 ray_color(const FRay& ray, const glm::vec3& cbegin, const glm::vec3& cend)
{
	auto direction = glm::normalize(ray.m_direction);
	auto t = 0.5f * (direction.y + 1.0f);
	//return glm::mix(cbegin, cend, t);
	//return glm::vec3(glm::vec3(0.01f, 0.01f, 0.03f) * (1.f - t) + glm::vec3(0.0f, 0.0f, 0.0f) * t); // night
	return glm::vec3(0.f);
}

CIntegrator::CIntegrator(CResourceManager* resource_manager)
{
	m_pResourceManager = resource_manager;
}

void CIntegrator::create(uint32_t width, uint32_t heigth, uint32_t samples)
{
	auto& config = CConfiguration::getInstance()->get();

	// Creating a framebuffer
	m_pFramebuffer = std::make_unique<CFramebuffer>(m_pResourceManager);
	m_pFramebuffer->create(width, heigth);

	m_sky_begin = config.m_scfg.m_skybox.m_gradient.m_begin;
	m_sky_end = config.m_scfg.m_skybox.m_gradient.m_end;

	m_sampleCount = config.m_icfg.m_sample_count;
	m_bounceCount = config.m_icfg.m_bounce_count;
	m_rrThreshold = config.m_icfg.m_rr_threshold;

	m_use_estimator = config.m_icfg.m_use_estimator;
	m_estimator_tolerance = config.m_icfg.m_estimator_tolerance;

	m_pixelIterator.resize(width * heigth);
	std::iota(m_pixelIterator.begin(), m_pixelIterator.end(), 0u);
}

void CIntegrator::trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin)
{
	//std::execution::par, 
	std::for_each(std::execution::par, m_pixelIterator.begin(), m_pixelIterator.end(),
		[this, scene, camera, &origin](uint32_t index)
		{
			trace_ray(scene, camera, origin, index);
		});
}

void CIntegrator::trace_ray(CScene* scene, FCameraComponent* camera, const glm::vec3& origin, uint32_t ray_index)
{
	auto& viewport_extent = camera->m_viewportExtent;

	auto x = ray_index % viewport_extent.x;
	auto y = ray_index / viewport_extent.x;

	auto& ray_direction = camera->m_vRayDirections[ray_index];

	static thread_local CCMGSampler sampler(m_sampleCount);
	sampler.begin(ray_index);

	uint32_t actual_sample_count{ 0u };
	float s1{ 0.f }, s2{ 0.f };

	glm::vec4 final_color{ 0.f };
	while (true)
	{
		FRay ray{};
		ray.m_origin = origin;
		ray.set_direction(ray_direction);// +glm::vec3(sampler.sample(-0.001f, 0.001f), sampler.sample(-0.001f, 0.001f), sampler.sample(-0.001f, 0.001f)));

		glm::vec4 sampled_color = glm::vec4(integrate(scene, ray, m_bounceCount, sampler), 1.f);
		final_color += sampled_color;

		++actual_sample_count;
		sampler.next();

		// based on https://cs184.eecs.berkeley.edu/sp21/docs/proj3-1-part-5
		if (m_use_estimator)
		{
			float luma = glm::dot(glm::vec3(0.299f, 0.587f, 0.114f), glm::vec3(sampled_color));
			s1 += luma;
			s2 += luma * luma;

			if (!(actual_sample_count % 10u))
			{
				float n = static_cast<float>(actual_sample_count);

				float u = s1 / n;
				float deviation = glm::sqrt(1.f / (n - 1.f) * (s2 - s1 * s1 / n));

				float I = 1.96f * deviation / glm::sqrt(n);

				if (I <= m_estimator_tolerance * u)
					break;
			}
		}
		else
		{
			if (actual_sample_count >= m_sampleCount)
				break;
		}
	}

	m_pFramebuffer->add_pixel(x, y, final_color / static_cast<float>(actual_sample_count));
}

glm::vec3 CIntegrator::integrate(CScene* scene, FRay ray, int32_t bounces, CCMGSampler& sampler)
{
	glm::vec3 throughput{ 1.f };
	glm::vec3 out_color{ 0.f };

	FHitResult hit_result{};
	auto hit_something = scene->trace_ray(ray, 0.001f, std::numeric_limits<float>::infinity(), hit_result);

	for (uint32_t depth = 0u; depth <= bounces; ++depth)
	{
		if (!hit_something)
		{
			out_color += throughput * ray_color(ray, m_sky_begin, m_sky_end);
			break;
		}

		auto& material = m_pResourceManager->get_material(hit_result.m_material_id);

		if (material->can_emit_light() && depth == 0)
		{
			out_color += throughput * material->emit(hit_result);

			if(!material->can_scatter_light())
				break;
		}

		float alpha{ 1.f };
		glm::vec3 diffuse = material->sample_diffuse_color(hit_result, alpha);
		glm::vec2 mr = material->sample_surface_metallic_roughness(hit_result);
		glm::vec3 normal = material->sample_surface_normal(hit_result);
		COrthonormalBasis basis(normal);

		bool is_transparent = alpha < sampler.sample();

		glm::vec3 wo = basis.to_local(glm::normalize(-ray.m_direction));

		size_t light_index{ invalid_index };
		const CTriangle* light{ nullptr };

		// Calculate direct illumination if we have lights
		float light_probability = scene->get_light_probability();
		if (!std::isinf(light_probability))
		{
			light_index = scene->get_light_index(sampler.sample());
			light = &scene->get_light(light_index);
		
			float light_pdf{};
			glm::vec3 light_dir = light->sample(hit_result.m_position, sampler.sample_vec2(), light_pdf);
			glm::vec3 wi = basis.to_local(light_dir);
		
			float cos_theta_i = glm::abs(cos_theta(wi));
			if (cos_theta_i > 0.f && light_pdf > 0.f)
			{
				FRay direct_ray;
				direct_ray.m_origin = hit_result.m_position + glm::sign(cos_theta(wi)) * normal * 0.001f;
				if (is_transparent)
					direct_ray.set_direction(ray.m_direction);
				else
					direct_ray.set_direction(light_dir);
		
				FHitResult direct_hit{};
				bool direct_hit_something = scene->trace_ray(direct_ray, 0.001f, std::numeric_limits<float>::infinity(), direct_hit);

				// If we hit something transparent we should go to the next step
				if(is_transparent)
				{
					ray = direct_ray;
					hit_result = direct_hit;
					hit_something = direct_hit_something;
					continue;
				}

				if (direct_hit_something && light_index == direct_hit.m_primitive_id && direct_hit.m_primitive_id != hit_result.m_primitive_id)
				{
					float bsdf_pdf = material->pdf(wi, wo, diffuse, mr);
					if (bsdf_pdf > 0.f)
					{
						auto& light_material = m_pResourceManager->get_material(light->get_material_id());
						auto emittance = light_material->emit(direct_hit);
						auto bsdf = material->eval(wi, wo, diffuse, mr);
						float weight = balance_heuristic(light_pdf, bsdf_pdf);
						out_color += throughput * emittance * bsdf * cos_theta_i * weight / (light_pdf * light_probability);
					}
				}
			}
		}

		// Calculate indirect light
		float bsdf_pdf{};
		glm::vec3 wi = material->sample(wo, sampler.sample_vec2(), diffuse, mr, bsdf_pdf);
		float cos_theta_i = glm::abs(cos_theta(wi));
		if (cos_theta_i <= 0.f || bsdf_pdf <= 0.f)
			break;

		glm::vec3 bsdf = material->eval(wi, wo, diffuse, mr);

		FRay indirect_ray{};
		indirect_ray.m_origin = hit_result.m_position + glm::sign(cos_theta(wi)) * normal * 0.001f;
		indirect_ray.set_direction(basis.to_world(wi));

		FHitResult indirect_hit{};
		bool indirect_hit_something = scene->trace_ray(indirect_ray, 0.001f, std::numeric_limits<float>::infinity(), indirect_hit);
		if (indirect_hit_something && light_index == indirect_hit.m_primitive_id && indirect_hit.m_primitive_id != hit_result.m_primitive_id)
		{
			if (light)
			{
				float light_pdf = light->pdf(hit_result.m_position, indirect_ray.m_direction);
				if (light_pdf > 0.f)
				{
					auto& light_material = m_pResourceManager->get_material(light->get_material_id());
					auto emittance = light_material->emit(indirect_hit);
					float weight = balance_heuristic(bsdf_pdf, light_pdf);
					out_color += throughput * emittance * bsdf * cos_theta_i * weight / (bsdf_pdf * light_probability);
				}
			}
		}

		throughput *= bsdf * cos_theta_i / bsdf_pdf;

		float rr_prob = glm::min(0.95f, math::max_component(throughput));
		if (depth >= m_rrThreshold)
		{
			if (sampler.sample() > rr_prob)
				break;

			throughput /= rr_prob;
		}

		ray = indirect_ray;
		hit_result = indirect_hit;
		hit_something = indirect_hit_something;
	}

	return out_color;
}

const std::unique_ptr<CFramebuffer>& CIntegrator::get_framebuffer() const
{
	return m_pFramebuffer;
}

const std::vector<uint32_t>& CIntegrator::get_pixel_iterator() const
{
	return m_pixelIterator;
}