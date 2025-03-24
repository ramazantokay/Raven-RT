#pragma once

#include "Math/glm/glm.hpp"
using glm::vec3;

namespace raven
{
	enum MaterialType
	{
		Simple,
		Mirror,
		Dielectric,
		Conductor
	};

	class Material
	{
	public:
		Material() : m_ambient_reflectance(vec3(0.0f)), m_diffuse_reflectance(vec3(0.0f)), m_specular_reflectance(vec3(0.0f)), m_mirror_reflectance(vec3(0.0f)),
			m_refraction_index(0.0f), m_absorption_index(0.0f), m_absorption_coefficient(vec3(0.0f)),
			m_phong_exp(0), m_roughness(0.0f), m_type(MaterialType::Simple), m_degamma(false), m_brdf_id(0) {}
	public:
		vec3 m_ambient_reflectance; 
		vec3 m_diffuse_reflectance;
		vec3 m_specular_reflectance;
		vec3 m_mirror_reflectance;
		vec3 m_absorption_coefficient;
		float m_refraction_index;
		float m_absorption_index;
		float m_phong_exp;
		float m_roughness;
		MaterialType m_type;
		bool m_degamma;
		int m_brdf_id;
	};
}