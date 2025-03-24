#include "ravenpch.h"
#include "Light.h"

vec3 PointLight::calculate_wi(const vec3& intersection_point, const vec3& normal)
{
	vec3 wi = glm::normalize(m_position - intersection_point);
	return wi;
}

float PointLight::calculate_distance(const vec3 intersection_point)
{
	return glm::length(m_position - intersection_point);
}

vec3 PointLight::calculate_E(const vec3& intersection_point)
{
	float dist = calculate_distance(intersection_point);
	vec3 E = m_intensity / (dist * dist);
	return E;
}
