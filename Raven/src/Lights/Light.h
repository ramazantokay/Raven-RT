#pragma once
#include "Math/glm/glm.hpp"
#include "Core/ImageTexture.h"
#include "Math/ONB.h"
#include <random>
#include <chrono>

using glm::vec3;

enum class SphericalDirectionalLightType
{
	None,
	LatLong,
	Probe
};

class Light
{
public:
	virtual vec3 calculate_wi(const vec3 & intersection_point, const vec3 &normal) = 0;
	virtual float calculate_distance(const vec3 intersection_point) = 0;
	virtual vec3 calculate_E(const vec3& intersection_point) = 0; // irradiance
};

class PointLight : public Light
{
public:
	PointLight(vec3& position, vec3& intensity) : m_position(position), m_intensity(intensity) {}
	vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override;
	float calculate_distance(const vec3 intersection_point) override;
	vec3 calculate_E(const vec3& intersection_point) override;

public:
	vec3 m_position;
	vec3 m_intensity;
};

class AreaLight : public Light
{
public:
	AreaLight(vec3& position, vec3& normal, float _size, vec3& radiance) :
		m_position(position), m_normal(normal), m_size(_size), m_radiance(radiance)
	{
		m_frame.init_from_w(normal);
		area_light_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
		area_light_distribution = std::uniform_real_distribution<float>(-0.5f, 0.5f);
	}
	vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override;
	float calculate_distance(const vec3 intersection_point) override;
	vec3 calculate_E(const vec3& intersection_point) override;


public:
	vec3 m_position;
	vec3 m_normal;
	vec3 m_radiance;
	raven::ONB m_frame;
	float m_size; // extent

private:
	static thread_local vec3 w_i;
	static thread_local vec3 position_on_light;
	std::mt19937 area_light_generator;
	std::uniform_real_distribution<float> area_light_distribution;
};

class DirectionalLight : public Light
{
public:
	DirectionalLight(vec3& direction, vec3& radiance) : m_direction(direction), m_radiance(radiance) {}
	vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override { return glm::normalize(-m_direction); }
	float calculate_distance(const vec3 intersection_point) override { return raven_infinity; }
	vec3 calculate_E(const vec3& intersection_point) override { return m_radiance; }

public:
	vec3 m_direction;
	vec3 m_radiance;

};

class SpotLight : public Light
{
public:
	SpotLight(vec3& position, vec3& direction, vec3& intensity, float coverage_angle, float falloff_angle) :
		m_position(position), m_direction(direction), m_intensity(intensity), m_coverage_angle(coverage_angle), m_falloff_angle(falloff_angle) {}
	
	vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override { return glm::normalize(m_position - intersection_point); }
	float calculate_distance(const vec3 intersection_point) override { return glm::length(m_position - intersection_point); }
	
	vec3 calculate_E(const vec3& intersection_point) override 
	{
		float fall_off = calculate_falloff(intersection_point);
		float dist = calculate_distance(intersection_point);
		vec3 E = (m_intensity / (dist * dist)) * fall_off;
		return E;
	}
	
private:
	float calculate_falloff(const vec3& intersection_point)
	{
		vec3 wi = glm::normalize(m_position - intersection_point);
		float cos_theta = dot(-wi, normalize(m_direction));
		float theta = acos(cos_theta);

		if (theta < (m_falloff_angle / 2.0f)) // No falloff
		{
			return 1.0f;
		}

		if (theta > (m_coverage_angle / 2.0f)) // No light
		{
			return 0.0f;
		}

		float cos_falloff_angle = cos(m_falloff_angle / 2.0f);
		float cos_coverage_angle = cos(m_coverage_angle / 2.0f);

		float intensity_falloff = (cos_theta - cos_coverage_angle) / (cos_falloff_angle - cos_coverage_angle);
		intensity_falloff = pow(intensity_falloff, 4);

		return intensity_falloff;
	}


public:
	vec3 m_position;
	vec3 m_direction;
	vec3 m_intensity;
	float m_coverage_angle;
	float m_falloff_angle;
};

class SphericalDirectionalLight : public Light
{
public:
	SphericalDirectionalLight(const std::string& file_path, SphericalDirectionalLightType type)
	{
		m_env_map = new raven::ImageTexture(file_path, "nearest", "replace_kd", 255.0f, 1.0f);
		m_type = type;
		m_sample_points = vec3(0.0f);

		spherical_directional_light_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
		spherical_directional_light_distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);

	}
	vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override;
	float calculate_distance(const vec3 intersection_point) override;
	vec3 calculate_E(const vec3& l_dir) override;

public:
	raven::ImageTexture* m_env_map;
	SphericalDirectionalLightType m_type;

private:
	static thread_local vec3 m_sample_points;
	std::default_random_engine spherical_directional_light_generator;
	std::uniform_real_distribution<float> spherical_directional_light_distribution;
};

