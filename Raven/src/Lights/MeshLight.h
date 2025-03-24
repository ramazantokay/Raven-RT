#pragma once

#include "Math/glm/glm.hpp"
#include "Light.h"
#include "Shapes/Mesh.h"
//#include "Math/Transformations.h"

using glm::vec3;
using glm::vec4;

namespace raven
{
	class Transformations;

	class MeshLight : public Light, public Mesh
	{
	public:

		MeshLight(int material_id, raven::Texture* tex, raven::Texture* normal,
			std::vector<Shape*>& triangles, raven::Transformations transformation, const vec3& radiance);

		vec3 calculate_wi(const vec3& intersection_point, const vec3& normal) override;
		float calculate_distance(const vec3 intersection_point) override;
		vec3 calculate_E(const vec3& intersection_point) override;
		bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;
		void calculate_cdf();


	public:
		vec3 m_radiance;

	private:
		static thread_local vec3 m_position_on_light;
		static thread_local vec3 m_wi;
		static thread_local float m_pw;

		std::vector<float> m_cdf;
		float m_total_area;
		std::mt19937 m_mesh_light_generator;
		std::uniform_real_distribution<float> m_mesh_light_distribution;
	};

}