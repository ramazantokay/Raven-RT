#pragma once

#include "ravenpch.h"
//#include "Triangle.h"
#include "Shape.h"
#include "BVH.h"
//#include "Math/Transformations.h"
//#include "Core/Texture.h"

namespace raven
{
	class Texture;
	class Transformations;

	class Mesh : public Shape
	{
	public:
		// Constructors
		Mesh(int material_id, std::vector<Shape*>& triangles) :m_material_id(material_id), m_bvh(NULL), m_triangless(triangles)
		{
			m_bvh = BVH::create(triangles);
			m_specular_texture = nullptr;
			m_normal_texture = nullptr;
			m_texture = nullptr;
		}

		Mesh(int material_id, raven::Texture* tex, raven::Texture* normal, std::vector<Shape*>& triangles, Transformations transformation) :
			m_material_id(material_id), m_texture(tex), m_normal_texture(normal), m_bvh(NULL), m_triangless(triangles)
		{
			m_bvh = BVH::create(triangles);
			m_transformation = transformation;
			m_specular_texture = nullptr;
		}

		/*~Mesh()
		{
			if (m_bvh) delete m_bvh;
		}*/


		// Virtual Functions
		bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override
		{
			return m_bvh->hit(ray, t_min, t_max, rec);
		}

		bool shadow_hit(const Ray& ray, float t_min, float t_max, float time) const override { return false; }
		int get_material_id() const override { return m_material_id; }
		int get_texture_id() const override { return -1; }
		const BBox& get_bbox() const override { return m_bvh->get_bbox(); }

		// Variables
	public:
		int m_material_id;
		raven::Texture* m_texture;
		raven::Texture* m_normal_texture; // Add this to Shape class
		raven::Texture* m_specular_texture; // Add this to Shape class
		Shape* m_bvh;
		Transformations m_transformation;
		std::vector<Shape*> m_triangless;
	};

}