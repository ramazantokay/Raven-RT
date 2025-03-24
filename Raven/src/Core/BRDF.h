#pragma once
#include "ravenpch.h"
#include "Math/glm/glm.hpp"
#include "Core/HitRecord.h"
#include "Core/Material.h"

using glm::vec3;

class BRDF
{
public:
	virtual vec3 get_reflectance(const HitRecord& hit, const raven::Material& material, const vec3& wo, const vec3& wi) const = 0;
};

class PhongBRDF : public BRDF
{
public:
	PhongBRDF(float exp) : m_exp(exp) {}

	vec3 get_reflectance(const HitRecord& hit, const raven::Material& material, const vec3& wo, const vec3& wi) const override
	{
		vec3 L = vec3(0.0f);
		float cos_theta = std::max(0.0f, glm::dot(hit.m_normal, wi));

		if (cos_theta > 0.0f)
		{
			vec3 w_r = glm::normalize(2.0f * (hit.m_normal * cos_theta) - wi);
			float cos_alpha = std::max(0.0f, glm::dot(wo, w_r));
			L = material.m_diffuse_reflectance + material.m_specular_reflectance * (pow(cos_alpha, m_exp) / cos_theta);
		}

		return L * cos_theta;
	}

public:
	float m_exp;
};

class ModifiedPhongBRDF : public BRDF
{
public:
	ModifiedPhongBRDF(float exp, bool normalized) : m_exp(exp), m_is_normalized(normalized) {}

	vec3 get_reflectance(const HitRecord& hit, const raven::Material& material, const vec3& wo, const vec3& wi) const override
	{
		vec3 L = vec3(0.0f);
		float cos_theta = std::max(0.0f, glm::dot(hit.m_normal, wi));

		if (cos_theta > 0.0f)
		{
			vec3 w_r = glm::normalize(2.0f * (hit.m_normal * cos_theta) - wi);
			float cos_alpha = std::max(0.0f, glm::dot(wo, w_r));

			if (m_is_normalized)
			{
				L = (material.m_diffuse_reflectance / RAVEN_PI) +
					(material.m_specular_reflectance * ((m_exp + 2) / (2 * RAVEN_PI)) *
						pow(cos_alpha, m_exp));
			}
			else
			{
				L = material.m_diffuse_reflectance +
					material.m_specular_reflectance *
					(pow(cos_alpha, m_exp) / cos_theta);
			}
		}

		return L * cos_theta;
	}

public:
	float m_exp;
	bool m_is_normalized;
};

class BlinnPhongBRDF : public BRDF
{
public:

	BlinnPhongBRDF(float exp) : m_exp(exp) {}

	vec3 get_reflectance(const HitRecord& hit, const raven::Material& material, const vec3& wo, const vec3& wi) const override
	{
		vec3 L = vec3(0.0f);
		float cos_theta = std::max(0.0f, glm::dot(hit.m_normal, wi));

		if (cos_theta > 0.0f)
		{
			vec3 w_h = glm::normalize(wi + wo);
			float cos_alpha = std::max(0.0f, glm::dot(hit.m_normal, w_h));
			L = material.m_diffuse_reflectance + material.m_specular_reflectance * (pow(cos_alpha, m_exp) / cos_theta);
		}

		return L * cos_theta;
	}

public:
	float m_exp;
};

class ModifiedBlinnPhongBRDF : public BRDF
{
public:

	ModifiedBlinnPhongBRDF(float exp, bool normalized) : m_exp(exp), m_is_normalized(normalized) {}

	vec3 get_reflectance(const HitRecord& hit, const raven::Material& material, const vec3& wo, const vec3& wi) const override
	{
		vec3 L = vec3(0.0f);
		float cos_theta = std::max(0.0f, glm::dot(hit.m_normal, wi));

		if (cos_theta > 0.0f)
		{
			vec3 w_h = glm::normalize(wi + wo);
			float cos_alpha = std::max(0.0f, glm::dot(hit.m_normal, w_h));

			if (m_is_normalized)
			{
				L = (material.m_diffuse_reflectance / RAVEN_PI) +
					(material.m_specular_reflectance * ((m_exp + 8) / (8 * RAVEN_PI)) *
						pow(cos_alpha, m_exp));
			}
			else
			{
				L = material.m_diffuse_reflectance +
					material.m_specular_reflectance *
					(pow(cos_alpha, m_exp));
			}
		}

		return L * cos_theta;
	}

public:
	float m_exp;
	bool m_is_normalized;
};

class TorranceSparrowBRDF : public BRDF
{
public:
	TorranceSparrowBRDF(float exp, float is_kd) : m_exp(exp), m_is_kd_fresnel(is_kd) {}

	vec3 get_reflectance(const HitRecord& hit, const raven::Material& material, const vec3& wo, const vec3& wi) const override
	{
		vec3 L = vec3(0.0f);
		float cos_theta = std::max(0.0f, glm::dot(hit.m_normal, wi));
		float cos_phi = std::max(0.0f, glm::dot(hit.m_normal, wo));

		if (cos_theta > 0.0f)
		{
			vec3 w_h = glm::normalize(wi + wo);

			float cos_alpha = std::max(0.0f, glm::dot(hit.m_normal, w_h));
			float cos_beta = std::max(0.0f, glm::dot(wo, w_h));

			float D = (m_exp + 2) / (2 * RAVEN_PI) * pow(cos_alpha, m_exp);

			float G = std::min(1.0f, std::min(2.0f * cos_alpha * cos_phi / cos_beta, 2.0f * cos_alpha * cos_theta / cos_beta));

			float n = material.m_refraction_index;
			float k = material.m_absorption_index;

			// Fresnel
			float rp = ((n * n + k * k) * cos_beta * cos_beta - 2 * n * cos_beta + 1) / ((n * n + k * k) * cos_beta * cos_beta + 2 * n * cos_beta + 1);
			float rs = ((n * n + k * k) - 2 * n * cos_beta + cos_beta * cos_beta) / ((n * n + k * k) + 2 * n * cos_beta + cos_beta * cos_beta);

			float F = 0.5f * (rp + rs);

			L = (material.m_diffuse_reflectance / RAVEN_PI) + material.m_specular_reflectance * ((D * G * F) / (4 * cos_theta * (cos_phi + 0.00001f)));
		}

		return L * cos_theta;
	}
public:
	float m_exp;
	bool m_is_kd_fresnel;
};