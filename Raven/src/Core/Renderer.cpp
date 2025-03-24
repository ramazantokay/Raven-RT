#include "ravenpch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Renderer.h"
#include "Core/RGB.h"
#include "Core/HitRecord.h"
#include "Shapes/Mesh.h"
#include "Shapes/Triangle.h"
#include "Shapes/Sphere.h"
#include "Core/Material.h"
//#include "Core/PointLight.h"
#include "Shapes/BVH.h"
#include <random>
#include <chrono>

#include "Lights/Light.h"


Renderer::Renderer(raven::Scene* scene) : m_scene(scene) {}


vec3 Renderer::compute_color(const Ray& ray, raven::Camera cam, int p_x, int p_y, int recursion_depth)
{
	vec3 color(0.0f);

	HitRecord hit_record;
	hit_record.m_t = infinity;
	hit_record.m_shape = NULL;
	hit_record.m_texture = NULL;
	// for debug purpose
	bool hit_test = m_scene->m_bvh->hit(ray, INTERSECTION_EPSILON, raven_infinity, hit_record);
	if (!hit_test)
	{
		if (m_scene->m_max_recursion == recursion_depth)
		{
			if (m_scene->m_background_texture)
			{
				int u = p_x % m_scene->m_background_texture->m_width;
				int v = p_y % m_scene->m_background_texture->m_height;

				vec2 uv = vec2(u / float(m_scene->m_background_texture->m_width), v / float(m_scene->m_background_texture->m_height));

				return m_scene->m_background_texture->get_texture_color(uv);
			}
			else if (m_scene->m_spherical_directional_light)
			{
				// lat long map
				if (m_scene->m_spherical_directional_light->m_type == SphericalDirectionalLightType::LatLong)
				{
					float theta = acos(ray.direction().y);
					float phi = atan2(ray.direction().x, -ray.direction().z);

					float s_u = (1.0f + (phi / RAVEN_PI)) / 2.0f;
					float s_v = theta / RAVEN_PI;

					return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));

				}
				else // probe map
				{
					float r = acos(-ray.direction().z) / RAVEN_PI;
					r = r / sqrt(ray.direction().x * ray.direction().x + ray.direction().y * ray.direction().y);

					float s_u = (ray.direction().x * r + 1.0f) / 2.0f;
					float s_v = (-ray.direction().y * r + 1.0f) / 2.0f;

					return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));
				}
			}
			else
			{
				return m_scene->m_background_color;
			}

		}

		if (m_scene->m_spherical_directional_light)
		{
			// lat long map
			if (m_scene->m_spherical_directional_light->m_type == SphericalDirectionalLightType::LatLong)
			{
				float theta = acos(ray.direction().y);
				float phi = atan2(ray.direction().x, -ray.direction().z);

				float s_u = (1.0f + (phi / RAVEN_PI)) / 2.0f;
				float s_v = theta / RAVEN_PI;

				return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));

			}
			else // probe map
			{
				float r = acos(-ray.direction().z) / RAVEN_PI;
				r = r / sqrt(ray.direction().x * ray.direction().x + ray.direction().y * ray.direction().y);

				float s_u = (ray.direction().x * r + 1.0f) / 2.0f;
				float s_v = (-ray.direction().y * r + 1.0f) / 2.0f;

				return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));

			}
		}
		return color;

	}

	vec3 intersection = (ray.at(hit_record.m_t));

	Material material;
	int texture_id = -1;
	raven::Texture* texture = nullptr;

	if (hit_record.m_shape != NULL)
	{
		material = m_scene->m_materials[hit_record.m_shape->get_material_id() - 1];
		texture = hit_record.m_texture;
		//texture_id = hit_record.m_shape->get_texture_id() - 1;
	}

	if (hit_record.m_is_light_shape == true)
	{
		color = hit_record.m_radiance;
		return color;
	}

	if (texture && texture->m_decal_mode == raven::DecalMode::ReplaceAll)
	{
		color = texture->get_texture_color(hit_record.m_uv);
		return color;
	}

	vec3 w_0 = normalize(ray.m_origin - intersection); // camera direction

	// Apply degamma
	// TODO: do it in scene parser
	if (cam.m_tonemap.m_type != raven::TonemapType::None && material.m_degamma == true)
	{
		material.m_ambient_reflectance = glm::pow(material.m_ambient_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
		material.m_diffuse_reflectance = glm::pow(material.m_diffuse_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
		material.m_specular_reflectance = glm::pow(material.m_specular_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
		material.m_mirror_reflectance = glm::pow(material.m_mirror_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
	}

	vec3 diffuse_color = material.m_diffuse_reflectance;

	if (texture)
	{
		// Check the texture type

		auto perlin_texture = dynamic_cast<PerlinTexture*>(texture);
		auto image_texture = dynamic_cast<raven::ImageTexture*>(texture);
		auto checkboard_texture = dynamic_cast<CheckboardTexture*>(texture);

		if (perlin_texture)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv, intersection);
		}
		else if (image_texture)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv);
		}
		else if (checkboard_texture)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv);
		}

		if (texture->m_decal_mode == raven::DecalMode::BlendKd)
		{
			diffuse_color = (diffuse_color) * 0.5 + (material.m_diffuse_reflectance) * 0.5;
		}

		if (texture->m_decal_mode == raven::DecalMode::ReplaceKd)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv);
		}
	}

	// ADS Shading
	if (!ray.m_is_in_medium)
	{
		// Ambient 
		color += material.m_ambient_reflectance * m_scene->m_ambient_light;

		for (auto light : m_scene->m_lights)
		{
			vec3 w_i = light->calculate_wi(intersection, hit_record.m_normal);
			float light_distance = light->calculate_distance(intersection);

			Ray shadow_ray(intersection + (m_scene->m_shadow_ray_epsilon * w_i), w_i);
			shadow_ray.m_motion_time = ray.m_motion_time;

			HitRecord shadow_hr;
			shadow_hr.m_t = raven_infinity;
			shadow_hr.m_shape = NULL;

			m_scene->m_bvh->hit(shadow_ray, INTERSECTION_EPSILON, raven_infinity, shadow_hr);
			if (shadow_hr.m_t < (light_distance - m_scene->m_shadow_ray_epsilon) && shadow_hr.m_light != light && shadow_hr.m_t > 0.0f)
				continue;

			vec3 light_irradiance = light->calculate_E(intersection);

			// BRDF
			if (material.m_brdf_id > 0)
			{
				const BRDF* brdf = m_scene->m_brdfs[material.m_brdf_id - 1];
				color += brdf->get_reflectance(hit_record, material, w_0, w_i) * light_irradiance;
			}
			else
			{
				// Diffuse
				float intensity_cos = std::max(0.0f, dot(w_i, hit_record.m_normal));
				color += (diffuse_color * light_irradiance * intensity_cos);

				// Specular
				float specular_cos_theta = fmax(0.0f, dot(normalize(hit_record.m_normal), normalize(w_0 + w_i)));
				color += (material.m_specular_reflectance * light_irradiance) * powf(specular_cos_theta, material.m_phong_exp);
			}
		}
	}

	// Reflectance (Mirror) (supports glossy)

	if (material.m_mirror_reflectance != vec3(0.0f) && material.m_type == raven::MaterialType::Mirror && recursion_depth > 0)
	{
		vec3 w_r = normalize(2.0f * (dot((hit_record.m_normal), (w_0)) * (hit_record.m_normal)) - (w_0));

		// Glossy
		if (material.m_roughness != 0.0f)
		{
			raven::ONB uvr;
			uvr.init_from_wr(w_r);

			// for Glossy
			thread_local static std::mt19937 glossy_generator;
			glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			thread_local static std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

			float rand_u = glossy_distribution(glossy_generator);
			float rand_v = glossy_distribution(glossy_generator);

			w_r = normalize(w_r + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
		}

		Ray reflect_ray(intersection + (w_r * 0.00298f), w_r); // Reflection ray offset by epsilon
		reflect_ray.m_motion_time = ray.m_motion_time;
		color += material.m_mirror_reflectance * compute_color((reflect_ray), cam, p_x, p_y, recursion_depth - 1);
	}

	// Conductor (supports glossy)

	if (material.m_type == raven::MaterialType::Conductor && recursion_depth > 0)
	{
		vec3 w_r = normalize(2.0f * (dot((hit_record.m_normal), (w_0)) * (hit_record.m_normal)) - (w_0));

		// Glossy
		if (material.m_roughness != 0.0f) // Glossy 
		{
			raven::ONB uvr;
			uvr.init_from_wr(w_r);

			// for Glossy
			thread_local static std::mt19937 glossy_generator;
			glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			thread_local static std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

			float rand_u = glossy_distribution(glossy_generator);
			float rand_v = glossy_distribution(glossy_generator);

			w_r = normalize(w_r + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
		}

		Ray reflect_ray(intersection + (w_r * 0.00298), w_r); // Reflection ray offset by epsilon
		reflect_ray.m_motion_time = ray.m_motion_time;
		// Fresnel Equation for conductor

		float f_cos_theta = dot(ray.direction(), hit_record.m_normal); // w_0
		float f_cos_theta_square = f_cos_theta * f_cos_theta;
		float two_n2_cos_theta = 2 * material.m_refraction_index * f_cos_theta;
		float n2_sq_k2_sq = (material.m_refraction_index * material.m_refraction_index) + (material.m_absorption_index * material.m_absorption_index);

		float r_s = (n2_sq_k2_sq - two_n2_cos_theta + f_cos_theta_square) / (n2_sq_k2_sq + two_n2_cos_theta + f_cos_theta_square);
		float r_p = (n2_sq_k2_sq * f_cos_theta_square - two_n2_cos_theta + 1) / (n2_sq_k2_sq * f_cos_theta_square + two_n2_cos_theta + 1);

		float fresnel_ratio = 0.5f * (r_s + r_p);

		color += fresnel_ratio * material.m_mirror_reflectance * compute_color(reflect_ray, cam, p_x, p_y, recursion_depth - 1);
	}

	// Dielectric (Refraction) glass etc. (supports glossy)

	if (material.m_type == raven::MaterialType::Dielectric && recursion_depth > 0)
	{
		vec3 w_r = normalize(2.0f * (dot((hit_record.m_normal), (w_0)) * (hit_record.m_normal)) - (w_0));  // reflection ray

		vec3 w_t(0.0f); // transmission direction
		vec3 k(0.0f); // absorption coefficient
		float medium = material.m_refraction_index;
		float cos_theta = 0.0f;
		bool is_total_internal_reflection = false;
		bool is_entering_ray;

		vec3 medium_direction = normalize(ray.direction());

		if (dot(medium_direction, hit_record.m_normal) < 0.0f) // entering case
		{
			// refract 
			refract_ray(medium_direction, hit_record.m_normal, medium, w_t);
			cos_theta = dot(-medium_direction, hit_record.m_normal);
			is_entering_ray = true;
			k = vec3(1.0f);
		}
		else // exiting case
		{
			k.x = beer_law(material.m_absorption_coefficient.x, hit_record.m_t);
			k.y = beer_law(material.m_absorption_coefficient.y, hit_record.m_t);
			k.z = beer_law(material.m_absorption_coefficient.z, hit_record.m_t);

			is_entering_ray = false;

			if (refract_ray(medium_direction, -hit_record.m_normal, 1.0f / medium, w_t))
			{
				cos_theta = dot(w_t, hit_record.m_normal);
			}
			else
			{
				is_total_internal_reflection = true;
			}
		}

		if (is_total_internal_reflection)
		{
			// Glossy 
			if (material.m_roughness != 0.0f)
			{
				raven::ONB uvr;
				uvr.init_from_wr(w_r);

				// for Glossy
				thread_local static std::mt19937 glossy_generator;
				glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
				thread_local static std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

				float rand_u = glossy_distribution(glossy_generator);
				float rand_v = glossy_distribution(glossy_generator);

				w_r = normalize(w_r + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
			}

			Ray reflect_ray(intersection + (w_r * 0.00298), w_r); // Reflection ray offset by epsilon
			reflect_ray.m_motion_time = ray.m_motion_time;
			reflect_ray.m_is_in_medium = true;

			color += k * compute_color(reflect_ray, cam, p_x, p_y, recursion_depth - 1);
		}
		else
		{
			float r_0 = (medium - 1.f) * (medium - 1.f) / ((medium + 1.f) * (medium + 1.f));
			float r = r_0 + (1.0f - r_0) * float(pow(1.f - cos_theta, 5)); // Schlick Approximation 

			Ray reflect_ray(intersection + (w_r * 0.00298f), w_r); // Reflection ray offset by epsilon
			reflect_ray.m_motion_time = ray.m_motion_time;
			reflect_ray.m_is_in_medium = !is_entering_ray;

			// Glossy 
			if (material.m_roughness != 0.0f)
			{
				raven::ONB uvr;
				uvr.init_from_wr(w_t);

				// for Glossy
				std::mt19937 glossy_generator;
				glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
				std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

				float rand_u = glossy_distribution(glossy_generator);
				float rand_v = glossy_distribution(glossy_generator);

				w_t = normalize(w_t + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
			}

			Ray transmit_ray(intersection + (w_t * m_scene->m_shadow_ray_epsilon), w_t);
			transmit_ray.m_motion_time = ray.m_motion_time;
			transmit_ray.m_is_in_medium = is_entering_ray;

			color += k * (r * compute_color(reflect_ray, cam, p_x, p_y, recursion_depth - 1) + (1.f - r) * compute_color(transmit_ray, cam, p_x, p_y, recursion_depth - 1));
		}
	}

	if (p_x == 500 && p_y == 250)
	{
		std::cerr << "Color at (500,250):  " << color.x << " " << color.y << " " << color.z << std::endl;
	}
	return color;
}


vec3 Renderer::compute_color_for_path_tracer(const Ray& ray, raven::Camera cam, int p_x, int p_y, int recursion_depth)
{
	vec3 color(0.0f);

	HitRecord hit_record;
	hit_record.m_t = infinity;
	hit_record.m_shape = NULL;
	hit_record.m_texture = NULL;
	// for debug purpose
	bool hit_test = m_scene->m_bvh->hit(ray, INTERSECTION_EPSILON, raven_infinity, hit_record);
	if (!hit_test)
	{
		if (m_scene->m_max_recursion == recursion_depth)
		{
			if (m_scene->m_background_texture)
			{
				int u = p_x % m_scene->m_background_texture->m_width;
				int v = p_y % m_scene->m_background_texture->m_height;

				vec2 uv = vec2(u / float(m_scene->m_background_texture->m_width), v / float(m_scene->m_background_texture->m_height));

				return m_scene->m_background_texture->get_texture_color(uv);
			}
			else if (m_scene->m_spherical_directional_light)
			{
				// lat long map
				if (m_scene->m_spherical_directional_light->m_type == SphericalDirectionalLightType::LatLong)
				{
					float theta = acos(ray.direction().y);
					float phi = atan2(ray.direction().x, -ray.direction().z);

					float s_u = (1.0f + (phi / RAVEN_PI)) / 2.0f;
					float s_v = theta / RAVEN_PI;

					return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));

				}
				else // probe map
				{
					float r = acos(-ray.direction().z) / RAVEN_PI;
					r = r / sqrt(ray.direction().x * ray.direction().x + ray.direction().y * ray.direction().y);

					float s_u = (ray.direction().x * r + 1.0f) / 2.0f;
					float s_v = (-ray.direction().y * r + 1.0f) / 2.0f;

					return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));
				}
			}
			else
			{
				return m_scene->m_background_color;
			}

		}

		if (m_scene->m_spherical_directional_light)
		{
			// lat long map
			if (m_scene->m_spherical_directional_light->m_type == SphericalDirectionalLightType::LatLong)
			{
				float theta = acos(ray.direction().y);
				float phi = atan2(ray.direction().x, -ray.direction().z);

				float s_u = (1.0f + (phi / RAVEN_PI)) / 2.0f;
				float s_v = theta / RAVEN_PI;

				return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));

			}
			else // probe map
			{
				float r = acos(-ray.direction().z) / RAVEN_PI;
				r = r / sqrt(ray.direction().x * ray.direction().x + ray.direction().y * ray.direction().y);

				float s_u = (ray.direction().x * r + 1.0f) / 2.0f;
				float s_v = (-ray.direction().y * r + 1.0f) / 2.0f;

				return m_scene->m_spherical_directional_light->m_env_map->get_texture_color(vec2(s_u, s_v));

			}
		}
		return color;

	}

	vec3 intersection = (ray.at(hit_record.m_t));

	Material material;
	int texture_id = -1;
	raven::Texture* texture = nullptr;

	if (hit_record.m_shape != NULL)
	{
		material = m_scene->m_materials[hit_record.m_shape->get_material_id() - 1];
		texture = hit_record.m_texture;
		//texture_id = hit_record.m_shape->get_texture_id() - 1;
	}

	if (hit_record.m_is_light_shape == true)
	{
		if ((cam.m_is_nee == true) && (ray.m_is_direct == false))
		{
			return color;
		}
		else
		{
			color = hit_record.m_radiance;
			return color;
		}
	}

	if (texture && texture->m_decal_mode == raven::DecalMode::ReplaceAll)
	{
		color = texture->get_texture_color(hit_record.m_uv);
		return color;
	}

	vec3 w_0 = normalize(ray.m_origin - intersection); // camera direction

	// Apply degamma
	// TODO: do it in scene parser
	if (cam.m_tonemap.m_type != raven::TonemapType::None && material.m_degamma == true)
	{
		material.m_ambient_reflectance = glm::pow(material.m_ambient_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
		material.m_diffuse_reflectance = glm::pow(material.m_diffuse_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
		material.m_specular_reflectance = glm::pow(material.m_specular_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
		material.m_mirror_reflectance = glm::pow(material.m_mirror_reflectance, glm::vec3(cam.m_tonemap.m_gamma));
	}

	vec3 diffuse_color = material.m_diffuse_reflectance;

	if (texture)
	{
		// Check the texture type

		auto perlin_texture = dynamic_cast<PerlinTexture*>(texture);
		auto image_texture = dynamic_cast<raven::ImageTexture*>(texture);
		auto checkboard_texture = dynamic_cast<CheckboardTexture*>(texture);

		if (perlin_texture)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv, intersection);
		}
		else if (image_texture)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv);
		}
		else if (checkboard_texture)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv);
		}

		if (texture->m_decal_mode == raven::DecalMode::BlendKd)
		{
			diffuse_color = (diffuse_color) * 0.5 + (material.m_diffuse_reflectance) * 0.5;
		}

		if (texture->m_decal_mode == raven::DecalMode::ReplaceKd)
		{
			diffuse_color = texture->get_texture_color(hit_record.m_uv);
		}
	}

	// ADS Shading
	if (!ray.m_is_in_medium)
	{
		// Ambient 
		color += material.m_ambient_reflectance * m_scene->m_ambient_light;

		// Next Event Estimation
		if (cam.m_is_nee)
		{
			for (auto light : m_scene->m_lights)
			{
				vec3 w_i = light->calculate_wi(intersection, hit_record.m_normal);
				float light_distance = light->calculate_distance(intersection);

				Ray shadow_ray(intersection + (m_scene->m_shadow_ray_epsilon * w_i), w_i);
				shadow_ray.m_motion_time = ray.m_motion_time;

				HitRecord shadow_hr;
				shadow_hr.m_t = raven_infinity;
				shadow_hr.m_shape = NULL;

				m_scene->m_bvh->hit(shadow_ray, INTERSECTION_EPSILON, raven_infinity, shadow_hr);
				if (shadow_hr.m_t < (light_distance - m_scene->m_shadow_ray_epsilon) && shadow_hr.m_light != light && shadow_hr.m_t > 0.0f)
					continue;

				vec3 light_irradiance = light->calculate_E(intersection);

				// BRDF
				if (material.m_brdf_id > 0)
				{
					const BRDF* brdf = m_scene->m_brdfs[material.m_brdf_id - 1];
					color += brdf->get_reflectance(hit_record, material, w_0, w_i) * light_irradiance;
				}
				else
				{
					// Diffuse
					float intensity_cos = std::max(0.0f, dot(w_i, hit_record.m_normal));
					color += (diffuse_color * light_irradiance * intensity_cos);

					// Specular
					float specular_cos_theta = fmax(0.0f, dot(normalize(hit_record.m_normal), normalize(w_0 + w_i)));
					color += (material.m_specular_reflectance * light_irradiance) * powf(specular_cos_theta, material.m_phong_exp);
				}
			}
		}

		// calculate indirect lighting

		{
			if (cam.m_is_russian_roulette == false && recursion_depth <= 0)
			{
				return color;
			}

			// sample direction
			thread_local static std::mt19937 sample_dir_generator;
			sample_dir_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			thread_local static std::uniform_real_distribution<float> sample_dir_distribution(0.0f, 1.0f);

			float rand_u = sample_dir_distribution(sample_dir_generator);
			float rand_v = sample_dir_distribution(sample_dir_generator);

			float phi = 2.0f * RAVEN_PI * rand_u;
			float theta = acos(rand_v);

			if (cam.m_is_importance_sampling)
			{
				theta = asin(sqrt(rand_v));
			}

			// ONB
			raven::ONB uvw;
			uvw.init_from_wr(hit_record.m_normal);

			// Calculate wi
			vec3 wi = glm::normalize(uvw.w * cos(theta) + uvw.v * sin(theta) * cos(phi) + uvw.u * sin(theta) * sin(phi));

			// check if the ray is killed with russian roulette

			float cos_theta = std::max(0.001f, dot(wi, hit_record.m_normal));
			float q = 1.0f - cos_theta;

			if (cam.m_is_russian_roulette)
			{
				float rnd = sample_dir_distribution(sample_dir_generator);
				if (rnd <= q)
				{
					return color;
				}
			}



			Ray sample_ray(intersection + (hit_record.m_normal * 0.001f), wi);
			sample_ray.m_is_direct = false;

			vec3 indirect_irradiance = compute_color_for_path_tracer(sample_ray, cam, p_x, p_y, recursion_depth - 1);

			// BRDF
			if (material.m_brdf_id > 0)
			{
				const BRDF* brdf = m_scene->m_brdfs[material.m_brdf_id - 1];
				color += brdf->get_reflectance(hit_record, material, w_0, wi) * indirect_irradiance;
			}
			else
			{
				// Diffuse
				float intensity_cos = std::max(0.0f, dot(wi, hit_record.m_normal));
				color += (diffuse_color * indirect_irradiance * intensity_cos);

				// Specular
				float specular_cos_theta = fmax(0.0f, dot(normalize(hit_record.m_normal), normalize(w_0 + wi)));
				color += (material.m_specular_reflectance * indirect_irradiance) * powf(specular_cos_theta, material.m_phong_exp);
			}

			float p_w = 1.0f / (2.0f * RAVEN_PI);
			if (cam.m_is_importance_sampling)
			{
				p_w = cos(theta) / RAVEN_PI;
			}

			color = color / p_w;

			if (cam.m_is_russian_roulette)
			{
				color = color / (1.0f - q);
			}
		}
	}

	// Reflectance (Mirror) (supports glossy)

	if (material.m_mirror_reflectance != vec3(0.0f) && material.m_type == raven::MaterialType::Mirror && recursion_depth > 0)
	{
		vec3 w_r = normalize(2.0f * (dot((hit_record.m_normal), (w_0)) * (hit_record.m_normal)) - (w_0));

		// Glossy
		if (material.m_roughness != 0.0f)
		{
			raven::ONB uvr;
			uvr.init_from_wr(w_r);

			// for Glossy
			thread_local static std::mt19937 glossy_generator;
			glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			thread_local static std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

			float rand_u = glossy_distribution(glossy_generator);
			float rand_v = glossy_distribution(glossy_generator);

			w_r = normalize(w_r + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
		}

		Ray reflect_ray(intersection + (w_r * 0.00298f), w_r); // Reflection ray offset by epsilon
		reflect_ray.m_motion_time = ray.m_motion_time;
		color += material.m_mirror_reflectance * compute_color_for_path_tracer((reflect_ray), cam, p_x, p_y, recursion_depth - 1);
	}

	// Conductor (supports glossy)

	if (material.m_type == raven::MaterialType::Conductor && recursion_depth > 0)
	{
		vec3 w_r = normalize(2.0f * (dot((hit_record.m_normal), (w_0)) * (hit_record.m_normal)) - (w_0));

		// Glossy
		if (material.m_roughness != 0.0f) // Glossy 
		{
			raven::ONB uvr;
			uvr.init_from_wr(w_r);

			// for Glossy
			thread_local static std::mt19937 glossy_generator;
			glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
			thread_local static std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

			float rand_u = glossy_distribution(glossy_generator);
			float rand_v = glossy_distribution(glossy_generator);

			w_r = normalize(w_r + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
		}

		Ray reflect_ray(intersection + (w_r * 0.00298), w_r); // Reflection ray offset by epsilon
		reflect_ray.m_motion_time = ray.m_motion_time;
		// Fresnel Equation for conductor

		float f_cos_theta = dot(ray.direction(), hit_record.m_normal); // w_0
		float f_cos_theta_square = f_cos_theta * f_cos_theta;
		float two_n2_cos_theta = 2 * material.m_refraction_index * f_cos_theta;
		float n2_sq_k2_sq = (material.m_refraction_index * material.m_refraction_index) + (material.m_absorption_index * material.m_absorption_index);

		float r_s = (n2_sq_k2_sq - two_n2_cos_theta + f_cos_theta_square) / (n2_sq_k2_sq + two_n2_cos_theta + f_cos_theta_square);
		float r_p = (n2_sq_k2_sq * f_cos_theta_square - two_n2_cos_theta + 1) / (n2_sq_k2_sq * f_cos_theta_square + two_n2_cos_theta + 1);

		float fresnel_ratio = 0.5f * (r_s + r_p);

		color += fresnel_ratio * material.m_mirror_reflectance * compute_color_for_path_tracer(reflect_ray, cam, p_x, p_y, recursion_depth - 1);
	}

	// Dielectric (Refraction) glass etc. (supports glossy)

	if (material.m_type == raven::MaterialType::Dielectric && recursion_depth > 0)
	{
		vec3 w_r = normalize(2.0f * (dot((hit_record.m_normal), (w_0)) * (hit_record.m_normal)) - (w_0));  // reflection ray

		vec3 w_t(0.0f); // transmission direction
		vec3 k(0.0f); // absorption coefficient
		float medium = material.m_refraction_index;
		float cos_theta = 0.0f;
		bool is_total_internal_reflection = false;
		bool is_entering_ray;

		vec3 medium_direction = normalize(ray.direction());

		if (dot(medium_direction, hit_record.m_normal) < 0.0f) // entering case
		{
			// refract 
			refract_ray(medium_direction, hit_record.m_normal, medium, w_t);
			cos_theta = dot(-medium_direction, hit_record.m_normal);
			is_entering_ray = true;
			k = vec3(1.0f);
		}
		else // exiting case
		{
			k.x = beer_law(material.m_absorption_coefficient.x, hit_record.m_t);
			k.y = beer_law(material.m_absorption_coefficient.y, hit_record.m_t);
			k.z = beer_law(material.m_absorption_coefficient.z, hit_record.m_t);

			is_entering_ray = false;

			if (refract_ray(medium_direction, -hit_record.m_normal, 1.0f / medium, w_t))
			{
				cos_theta = dot(w_t, hit_record.m_normal);
			}
			else
			{
				is_total_internal_reflection = true;
			}
		}

		if (is_total_internal_reflection)
		{
			// Glossy 
			if (material.m_roughness != 0.0f)
			{
				raven::ONB uvr;
				uvr.init_from_wr(w_r);

				// for Glossy
				thread_local static std::mt19937 glossy_generator;
				glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
				thread_local static std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

				float rand_u = glossy_distribution(glossy_generator);
				float rand_v = glossy_distribution(glossy_generator);

				w_r = normalize(w_r + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
			}

			Ray reflect_ray(intersection + (w_r * 0.00298), w_r); // Reflection ray offset by epsilon
			reflect_ray.m_motion_time = ray.m_motion_time;
			reflect_ray.m_is_in_medium = true;

			color += k * compute_color_for_path_tracer(reflect_ray, cam, p_x, p_y, recursion_depth - 1);
		}
		else
		{
			float r_0 = (medium - 1.f) * (medium - 1.f) / ((medium + 1.f) * (medium + 1.f));
			float r = r_0 + (1.0f - r_0) * float(pow(1.f - cos_theta, 5)); // Schlick Approximation 

			Ray reflect_ray(intersection + (w_r * 0.00298f), w_r); // Reflection ray offset by epsilon
			reflect_ray.m_motion_time = ray.m_motion_time;
			reflect_ray.m_is_in_medium = !is_entering_ray;

			// Glossy 
			if (material.m_roughness != 0.0f)
			{
				raven::ONB uvr;
				uvr.init_from_wr(w_t);

				// for Glossy
				std::mt19937 glossy_generator;
				glossy_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
				std::uniform_real_distribution<float> glossy_distribution(-0.5f, 0.5f);

				float rand_u = glossy_distribution(glossy_generator);
				float rand_v = glossy_distribution(glossy_generator);

				w_t = normalize(w_t + material.m_roughness * (rand_u * uvr.u + rand_v * uvr.v));
			}

			Ray transmit_ray(intersection + (w_t * m_scene->m_shadow_ray_epsilon), w_t);
			transmit_ray.m_motion_time = ray.m_motion_time;
			transmit_ray.m_is_in_medium = is_entering_ray;

			color += k * (r * compute_color_for_path_tracer(reflect_ray, cam, p_x, p_y, recursion_depth - 1) + (1.f - r) * compute_color_for_path_tracer(transmit_ray, cam, p_x, p_y, recursion_depth - 1));
		}
	}

	if (p_x == 500 && p_y == 250)
	{
		std::cerr << "Color at (500,250):  " << color.x << " " << color.y << " " << color.z << std::endl;
	}
	return color;
}


void Renderer::render(int camera_index, std::vector<vec3>& pixels, const int row, const int height)
{
	raven::Camera& camera = m_scene->m_cameras[camera_index];

	const int num_samples = camera.m_num_samples;

	int w = camera.m_image_width;
	int h = camera.m_image_height;

	if (num_samples == 1) // 
	{
		for (int j = row; j < h; j += height)
		{
			for (int i = 0; i < w; i++)
			{
				if (camera.m_renderer_type == raven::RendererType::RayTracing)
				{
					Ray r = camera.get_ray(i, j);
					vec3 color = compute_color(r, camera, i, j, m_scene->m_max_recursion);

					pixels[j * w + i] = color;
				}
				else if (camera.m_renderer_type == raven::RendererType::PathTracing)
				{
					Ray r = camera.get_ray(i, j);
					vec3 color = compute_color_for_path_tracer(r, camera, i, j, m_scene->m_max_recursion);

					pixels[j * w + i] = color;
				}
			}
		}
	}
	else // For Multi sampling
	{
		for (int j = row; j < h; j += height)
		{
			for (int i = 0; i < w; i++)
			{
				// Initialize random number generator

				if (camera.m_renderer_type == raven::RendererType::RayTracing)
				{
					// for Multisampling
					std::mt19937 multisampling_generator;
					multisampling_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
					std::uniform_real_distribution<float> multisampling_distribution(0.0f, 1.0f);

					// for DOF
					std::mt19937 dof_generator;
					dof_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
					std::uniform_real_distribution<float> dof_distribution(0.0f, 1.0f);

					std::mt19937 time_generator;
					time_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
					std::uniform_real_distribution<float> time_distribution(0.0f, 1.0f);

					int sqrt_num_samples = int(sqrt(num_samples));
					vec3 result_color = vec3(0.0f);
					vec3 color;

					// Jitter Sampling
					for (int x = 0; x < sqrt_num_samples; x++)
					{
						for (int y = 0; y < sqrt_num_samples; y++)
						{
							float random_x = multisampling_distribution(multisampling_generator);
							float random_y = multisampling_distribution(multisampling_generator);

							float dx = (x + random_x) / sqrt_num_samples;
							float dy = (y + random_y) / sqrt_num_samples;

							if (camera.m_aperture_size == 0.0f)
							{
								Ray r = camera.get_ray(i + dx - 0.5f, j + dy - 0.5f);
								r.m_motion_time = time_distribution(time_generator);
								color = compute_color(r, camera, i, j, m_scene->m_max_recursion);
							}
							else
							{
								float dof_random_x = dof_distribution(dof_generator);
								float dof_random_y = dof_distribution(dof_generator);

								Ray r = camera.get_dof_ray(i + dx - 0.5f, j + dy - 0.5f, dof_random_x, dof_random_y);
								r.m_motion_time = time_distribution(time_generator);
								color = compute_color(r, camera, i, j, m_scene->m_max_recursion);
							}
							result_color += color;
						}
					}

					// Box Filtering
					result_color /= num_samples;
					pixels[j * w + i] = result_color;

				}
				else if (camera.m_renderer_type == raven::RendererType::PathTracing)
				{
					// for Multisampling
					std::mt19937 multisampling_generator;
					multisampling_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
					std::uniform_real_distribution<float> multisampling_distribution(0.0f, 1.0f);

					// for DOF
					std::mt19937 dof_generator;
					dof_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
					std::uniform_real_distribution<float> dof_distribution(0.0f, 1.0f);

					std::mt19937 time_generator;
					time_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
					std::uniform_real_distribution<float> time_distribution(0.0f, 1.0f);

					int sqrt_num_samples = int(sqrt(num_samples));
					vec3 result_color = vec3(0.0f);
					vec3 color;

					// Jitter Sampling
					for (int x = 0; x < sqrt_num_samples; x++)
					{
						for (int y = 0; y < sqrt_num_samples; y++)
						{
							float random_x = multisampling_distribution(multisampling_generator);
							float random_y = multisampling_distribution(multisampling_generator);

							float dx = (x + random_x) / sqrt_num_samples;
							float dy = (y + random_y) / sqrt_num_samples;

							if (camera.m_aperture_size == 0.0f)
							{
								Ray r = camera.get_ray(i + dx - 0.5f, j + dy - 0.5f);
								r.m_motion_time = time_distribution(time_generator);
								color = compute_color_for_path_tracer(r, camera, i, j, m_scene->m_max_recursion);
							}
							else
							{
								float dof_random_x = dof_distribution(dof_generator);
								float dof_random_y = dof_distribution(dof_generator);

								Ray r = camera.get_dof_ray(i + dx - 0.5f, j + dy - 0.5f, dof_random_x, dof_random_y);
								r.m_motion_time = time_distribution(time_generator);
								color = compute_color_for_path_tracer(r, camera, i, j, m_scene->m_max_recursion);
							}
							result_color += color;
						}
					}

					// Box Filtering
					result_color /= num_samples;
					pixels[j * w + i] = result_color;
				}

			}
		}
	}
}



bool Renderer::refract_ray(const vec3& dir, const vec3& normal, const float refract_index, vec3& transmit_dir) const
{
	// refract 
	float medium_ratio = 1 / refract_index;
	float cos_theta = dot(-dir, normal);
	float square_root_part = 1.0f - (medium_ratio) * (medium_ratio) * (1.0f - float(pow(cos_theta, 2)));

	if (square_root_part < 0.0f)
		return false;

	transmit_dir = normalize((dir + normal * cos_theta) * (medium_ratio)-normal * sqrt(square_root_part));
	return true;
}
