#include "ravenpch.h"
#include "Light.h"


thread_local vec3 AreaLight::w_i;
thread_local vec3 AreaLight::position_on_light;

vec3 AreaLight::calculate_wi(const vec3& intersection_point, const vec3& normal)
{
	float random_u = area_light_distribution(area_light_generator);
	float random_v = area_light_distribution(area_light_generator);

	position_on_light = m_position + m_size * (m_frame.u * random_u + m_frame.v * random_v);

	w_i = glm::normalize(position_on_light - intersection_point);
	
	return w_i;
}

float AreaLight::calculate_distance(const vec3 intersection_point)
{
	return glm::length(position_on_light - intersection_point);
}

vec3 AreaLight::calculate_E(const vec3& intersection_point)
{
	float area = m_size * m_size;
	float light_dist = calculate_distance(intersection_point);
	float intensity_cos = abs(dot(-w_i, m_normal));

	float d_wi = (area * intensity_cos) / (light_dist * light_dist);
	vec3 E = m_radiance * d_wi;

	return E;

}