#include "ravenpch.h"
#include "BVH.h"

#define INTERSECTION_EPSILON 0.000001f

raven::BVH::BVH(std::vector<Shape*>& primitives, int _start, int _end, int _axis) : m_right(NULL), m_left(NULL)
{
	// calculating global BBox
	for (int i = _start; i < _end; i++)
		m_bbox.update_bounds(primitives[i]->get_bbox());

	const float temp_center = m_bbox.m_center[_axis];

	int mid_index = _start;

	for (int i = _start; i < _end; i++)
	{
		if (primitives[i]->get_bbox().m_center[_axis] < temp_center)
			std::swap(primitives[i], primitives[mid_index++]);
	}

	if (mid_index == _start || mid_index == _end)
		mid_index = _start + ((_end - _start) / 2 );

	if (_start + 1 == mid_index)
	{
		m_left = primitives[_start];
	}
	else
	{
		m_left = new BVH(primitives, _start, mid_index, (_axis + 1) % 3);
	}

	if (mid_index +1 == _end)
	{
		m_right = primitives[mid_index];
	}
	else
	{
		m_right = new  BVH(primitives, mid_index, _end, (_axis + 1) % 3);
	}
}

raven::BVH::~BVH()
{
	if (m_right) delete m_right;
	if (m_left) delete m_left;
}

bool raven::BVH::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	float t_bbox = m_bbox.intersect(ray);

	if (t_bbox < 0.0f || t_bbox == raven_infinity)
		return false;

	bool is_intersect = false;

	HitRecord left_hr;
	left_hr.m_t = raven_infinity;
	left_hr.m_shape = NULL;

	HitRecord right_hr; 
	right_hr.m_t = raven_infinity;
	right_hr.m_shape = NULL;

	if (m_left->hit(ray, t_min, t_max, left_hr) && left_hr.m_t > 0.0f && left_hr.m_t < rec.m_t)
	{
		rec = left_hr;
		is_intersect = true;
	}
	
	if (m_right->hit(ray, t_min, t_max, right_hr) && right_hr.m_t > 0.0f && right_hr.m_t < rec.m_t)
	{
		rec = right_hr;
		is_intersect = true;
	}

    return is_intersect;
}

bool raven::BVH::shadow_hit(const Ray& ray, float t_min, float t_max, float time) const
{
	return false;
}

Shape* raven::BVH::create(std::vector<Shape*>& primitives)
{
	int size = int(primitives.size());

	if (size == 0)
		return NULL;
	else if (size == 1)
		return primitives[0];
	else
		return new BVH(primitives, 0, size, 0);
}
