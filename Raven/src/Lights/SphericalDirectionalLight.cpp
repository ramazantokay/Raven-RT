#include "ravenpch.h"
#include "Light.h"

thread_local vec3 SphericalDirectionalLight::m_sample_points;

vec3 SphericalDirectionalLight::calculate_wi(const vec3& intersection_point, const vec3& normal)
{
	// Random Rejection Sampling
	while (true)
	{
		m_sample_points.x = 2.0f * spherical_directional_light_distribution(spherical_directional_light_generator) - 1.0f;
		m_sample_points.y = 2.0f * spherical_directional_light_distribution(spherical_directional_light_generator) - 1.0f;
		m_sample_points.z = 2.0f * spherical_directional_light_distribution(spherical_directional_light_generator) - 1.0f;

		if (glm::length(m_sample_points) <= 1.0f && glm::dot(m_sample_points, normal) > 0.0f)
		{
			m_sample_points = glm::normalize(m_sample_points);
			break;
		}
	}

	return m_sample_points;
}

float SphericalDirectionalLight::calculate_distance(const vec3 intersection_point)
{
	return raven_infinity;
}

vec3 SphericalDirectionalLight::calculate_E(const vec3& l_dir)
{
	// calculate the irradiance
	float theta = acos(l_dir.y);
	float phi = atan2(l_dir.x, -l_dir.z);

	float s_u = ((1.0f + (phi / RAVEN_PI)) / 2.0f);
	float s_v = theta / RAVEN_PI;

	vec3 E = m_env_map->get_texture_color(glm::vec2(s_u, s_v));

	return E * 2.0f * RAVEN_PI;
}