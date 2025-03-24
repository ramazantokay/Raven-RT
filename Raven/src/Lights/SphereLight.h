#pragma once

#include "Math/glm/glm.hpp"
#include "Light.h"
#include "Shapes/Sphere.h"

using glm::vec3;

class SphereLight : public Light, public Sphere
{

public:
	SphereLight(const vec3& _center, float _radius, const int& _mat,
		raven::Texture* _texture, raven::Texture* _normal,
		raven::Transformations _transformation, const vec3& _radiance)
		: Sphere(_center, _radius, _mat, _texture, _normal, _transformation), m_radiance(_radiance) 
	{
		m_sphere_light_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
		m_sphere_light_distribution = std::uniform_real_distribution<float>(0.0, 1.f);
	}



	vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override;
	float calculate_distance(const vec3 intersection_point) override;
	vec3 calculate_E(const vec3& intersection_point) override;
	bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;

public:

	vec3 m_radiance;
	

private:
	static thread_local float m_pw;
	static thread_local vec3 m_wi;
	static thread_local vec3 m_position_on_light;

	std::mt19937 m_sphere_light_generator;
	std::uniform_real_distribution<float> m_sphere_light_distribution;
};