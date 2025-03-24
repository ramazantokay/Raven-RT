#pragma once

#include "ravenpch.h"
#include "Math/glm/glm.hpp"
#include "Shape.h"

namespace raven
{
	class BVH : public Shape
	{
	public:
		BVH(std::vector<Shape*>& primitives, int _start, int _end, int _axis);
		~BVH();

		// Virtuals
		bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec)const override;
		bool shadow_hit(const Ray& ray, float t_min, float t_max, float time) const override;
		int get_material_id() const override { return -1; }
		int get_texture_id() const override { return -1; }
		const BBox& get_bbox() const override { return m_bbox; }

		// Helpers
		static Shape* create(std::vector<Shape*>& primitives);

	public:
		Shape* m_left;
		Shape* m_right;
		BBox m_bbox;
	};
}
