#pragma once

#include "Shape.h"
#include "Mesh.h"
#include "BBox.h"
#include "Math/Transformations.h"
#include "Core/Texture.h"

//class Mesh;
//using raven::Mesh;

namespace raven
{
	class MeshInstance : public Shape
	{
	public:
		//MeshInstance() {}

		MeshInstance(int _material_id, raven::Texture *tex, raven::Texture* normal, raven::Mesh* _mesh, raven::Transformations _transformation, bool _reset_transform, vec3 motion_vector = vec3(0.0f)):
			m_material_id(_material_id), m_texture(tex), m_normal_texture(normal), m_mesh(_mesh), m_transformation(_transformation), m_motion_vector(motion_vector)
		{	
			m_is_reset_transform = (m_transformation.m_transformations == glm::mat4(1.0f)) ? false : true;

			BBox temp = BBox(_mesh->get_bbox().m_min, _mesh->get_bbox().m_max);

			// print BBox values for debugging

			std::cerr << "First BBox Values" << std::endl;
			std::cerr << "First BBox Min " << temp.m_min << std::endl;
			std::cerr << "First BBox Max " << temp.m_max << std::endl;
			std::cerr << "First BBox Center " << temp.m_center << std::endl;
			
			m_bbox = temp.apply_transformation(m_transformation);

			std::cerr << "After Apply  Transformation BBox Values " << std::endl;
			std::cerr << "BBox Min " << m_bbox.m_min << std::endl;
			std::cerr << "BBox Max " << m_bbox.m_max << std::endl;
			std::cerr << "BBox Center " << m_bbox.m_center << std::endl;

			if (m_motion_vector != vec3(0.0f)) // Motion Blur
			{
				vec3 _min = m_bbox.m_min;
				vec3 _max = m_bbox.m_max;
				m_bbox.update_bounds(BBox(_min + m_motion_vector, _max + m_motion_vector));
				m_bbox.update_bounds(BBox(_min - m_motion_vector, _max - m_motion_vector));
			}

			m_specular_texture = nullptr;
		}

		bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override
		{

			float t_bbox = m_bbox.intersect(ray);

			if (t_bbox < 0.0f || t_bbox == raven_infinity)
				return false;


			if (m_motion_vector == vec3(0.0f)) // No Motion Blur
			{
				glm::vec3 new_origin = glm::vec3{ glm::mat4(m_transformation.m_inverse_transformations) * glm::vec4(ray.m_origin, 1.0f) };
				glm::vec3 new_dir = glm::vec3{ (glm::mat4(m_transformation.m_inverse_transformations) * glm::vec4(ray.m_direction, 0.0f)) };

				Ray local_ray(new_origin, (new_dir));

				if (m_mesh->hit(local_ray, t_min, t_max, rec))
				{
					rec.m_shape = this;
					rec.m_normal = normalize(glm::vec3{ glm::mat4(m_transformation.m_normal_transformations) * glm::vec4(rec.m_normal, 0.0f) });
					return true;
				}
				return false;
			}
			else // Motion Blur
			{
				vec3 delta_position = ray.m_motion_time * m_motion_vector;
				
				vec3 temp_translation_vec(delta_position.x, delta_position.y, delta_position.z);

				Transformations temp_translation(44, TransformationType::Translation, temp_translation_vec);

				Transformations temp_transformation(temp_translation.m_transformations * m_transformation.m_transformations);

				glm::vec3 new_origin = glm::vec3{ glm::mat4(temp_transformation.m_inverse_transformations) * glm::vec4(ray.m_origin, 1.0f) };
				glm::vec3 new_dir = glm::vec3{ (glm::mat4(temp_transformation.m_inverse_transformations) * glm::vec4(ray.m_direction, 0.0f)) };

				Ray local_ray(new_origin, (new_dir));
				local_ray.m_motion_time = ray.m_motion_time;

				if (m_mesh->hit(local_ray, t_min, t_max, rec))
				{
					rec.m_shape = this;
					rec.m_normal = normalize(glm::vec3{ glm::mat4(temp_transformation.m_normal_transformations) * glm::vec4(rec.m_normal, 0.0f) });
					return true;
				}
				return false;

			}
		}

		bool shadow_hit(const Ray& ray, float t_min, float t_max, float time) const override { return false; }
		int get_material_id() const override { return m_material_id; }
		int get_texture_id() const override { return -1; }
		const BBox& get_bbox() const override { return m_bbox; }

	public:
		int m_material_id;
		raven::Texture* m_texture;
		raven::Texture* m_normal_texture; // Add this to Shape class
		raven::Texture* m_specular_texture; // Add this to Shape class
		raven::Mesh* m_mesh;
		BBox m_bbox;
		raven::Transformations m_transformation;
		bool m_is_reset_transform;
		vec3 m_motion_vector;
	};
}