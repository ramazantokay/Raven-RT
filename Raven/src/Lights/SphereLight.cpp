#include "ravenpch.h"
#include "SphereLight.h"
#include "Math/ONB.h"
#include "Core/Ray.h"
#include "Core/HitRecord.h"
#include "Helper.h"

thread_local float SphereLight::m_pw;
thread_local vec3 SphereLight::m_wi;
thread_local vec3 SphereLight::m_position_on_light;

vec3 SphereLight::calculate_wi(const vec3& intersection_point, const vec3& normal)
{
	vec3 local_position = glm::vec3{ glm::mat4(m_transformation.m_inverse_transformations) * glm::vec4(intersection_point, 1.0f) };
	vec3 w = m_center - local_position;
	float d = glm::length(w);

	float sin_theta_max = m_radius / d;
	float cos_theta_max = sqrt(std::max(0.0f, 1.0f - sin_theta_max * sin_theta_max));

	m_pw = 1.0f / (2.0f * RAVEN_PI * (1.0f - cos_theta_max));

	// ONB
	raven::ONB uvw;
	uvw.init_from_wr(w);

	float random_u = m_sphere_light_distribution(m_sphere_light_generator);
	float random_v = m_sphere_light_distribution(m_sphere_light_generator);

	float phi = 2.0f * RAVEN_PI * random_u;
	float theta = acos(1.0f - random_v + random_v * cos_theta_max);

	vec3 local_l = w * cos(theta) + uvw.v * sin(theta) * cos(phi) + uvw.u * sin(theta) * sin(phi);

	vec3 world_l = glm::vec3{ m_transformation.m_transformations * glm::vec4(local_l, 0.0f) };

	Ray light_ray(intersection_point + 0.001f * glm::normalize(world_l), glm::normalize(world_l));
	HitRecord rec;

	if (hit(light_ray, 0.000001f, raven_infinity, rec))
	{
		m_position_on_light = light_ray.at(rec.m_t);
		m_wi = glm::normalize(m_position_on_light - intersection_point);
	}

	return m_wi;
}

float SphereLight::calculate_distance(const vec3 intersection_point)
{
	return glm::length(m_position_on_light - intersection_point);
}

vec3 SphereLight::calculate_E(const vec3& intersection_point)
{
	return m_radiance / m_pw;
}

bool SphereLight::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	bool res = Sphere::hit(ray, t_min, t_max, rec);
	rec.m_radiance = m_radiance;
	rec.m_is_light_shape = true;
	rec.m_light = (Light*)this;

	return res;
}
