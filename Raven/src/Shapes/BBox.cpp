#include "ravenpch.h"
#include "BBox.h"

void raven::BBox::update_bounds(const BBox& bbox)
{
	m_min.x = fmin(m_min.x, bbox.m_min.x);
	m_min.y = fmin(m_min.y, bbox.m_min.y);
	m_min.z = fmin(m_min.z, bbox.m_min.z);

	m_max.x = fmax(m_max.x, bbox.m_max.x);
	m_max.y = fmax(m_max.y, bbox.m_max.y);
	m_max.z = fmax(m_max.z, bbox.m_max.z);

	m_center = (m_max + m_min) * 0.5f;
	m_delta = m_max - m_min;
}

float raven::BBox::intersect(const Ray& ray) const // TODO: return bool, refactor it,optimize it with https://tavianator.com/2015/ray_box_nan.html
{
	float t_min = -raven_infinity;
	float t_max = raven_infinity;

	const vec3& ray_direction = ray.m_direction;
	const vec3& ray_origin = ray.m_origin;

	// for calculating t for x,y,z axis
	for (int i = 0; i < 3; i++) // TODO: check here again! unroll it
	{
		if (fabs(ray_direction[i]) < 0.00001f) // TODO: add epsilon to a const var
			continue;

		float ti_min = (m_min[i] - ray_origin[i]) / ray_direction[i]; // TODO: use inv_ray_dir
		float ti_max = (m_max[i] - ray_origin[i]) / ray_direction[i];

		if (ray_direction[i] < 0.0f)
			std::swap(ti_min, ti_max);

		if (ti_min > t_min) t_min = ti_min;
		if (ti_max < t_max) t_max = ti_max;

		if (t_min > t_max)
			return raven_infinity;
	}

	return (t_min > 0.0f) ? t_min : t_max;

	//float inv_dir = 1.0f / ray.m_direction[0];

	//float t1 = (m_min[0] - ray.m_origin[0]) *inv_dir;
	//float t2 = (m_max[0] - ray.m_origin[0]) *inv_dir;

	//float t_min = std::min(t1,t2);
	//float t_max = std::max(t1,t2);

	//inv_dir = 1.0f / ray.m_direction[1];

	//t1 = (m_min[1] - ray.m_origin[1]) * inv_dir;
	//t2 = (m_max[1] - ray.m_origin[1]) * inv_dir;

	//t_min = std::max(t_min, std::min(t1, t2));
	//t_max = std::min(t_max, std::max(t1, t2));
	//
	//inv_dir = 1.0f / ray.m_direction[2];

	//t1 = (m_min[2] - ray.m_origin[2]) * inv_dir;
	//t2 = (m_max[2] - ray.m_origin[2]) * inv_dir;

	//t_min = std::max(t_min, std::min(t1, t2));
	//t_max = std::min(t_max, std::max(t1, t2));

	//for (int i = 1; i < 3; i++)
	//{
	//	//inv_dir = 1.0f / ray.m_direction[i];
	//	t1 = (m_min[i] - ray.m_origin[i])/ ray.m_direction[i]; //* inv_dir;
	//	t2 = (m_max[i] - ray.m_origin[i]) / ray.m_direction[i]; //* inv_dir;

	//	t_min = std::max(t_min, std::min(t1, t2));
	//	t_max = std::min(t_max, std::max(t1, t2));
	//}
	
	//return (t_min > 0.0f) ? t_min : t_max;

	//return t_max > std::max(t_min, 0.0f);

}

raven::BBox raven::BBox::apply_transformation(Transformations& transformation)
{
	const vec3 v000 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_min.x, m_min.y, m_min.z, 1.0f)) };
	const vec3 v001 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_min.x, m_min.y, m_max.z, 1.0f)) };
	const vec3 v010 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_min.x, m_max.y, m_min.z, 1.0f)) };
	const vec3 v011 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_min.x, m_max.y, m_max.z, 1.0f)) };

	const vec3 v100 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_max.x, m_min.y, m_min.z, 1.0f)) };
	const vec3 v101 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_max.x, m_min.y, m_max.z, 1.0f)) };
	const vec3 v110 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_max.x, m_max.y, m_min.z, 1.0f)) };
	const vec3 v111 = glm::vec3{ (glm::mat4(transformation.m_transformations) * glm::vec4(m_max.x, m_max.y, m_max.z, 1.0f)) };

	vec3 temp_min = v000;
	vec3 temp_max = v000;

	temp_min.x = fmin(temp_min.x, v001.x);
	temp_min.y = fmin(temp_min.y, v001.y);
	temp_min.z = fmin(temp_min.z, v001.z);

	temp_max.x = fmax(temp_max.x, v001.x);
	temp_max.y = fmax(temp_max.y, v001.y);
	temp_max.z = fmax(temp_max.z, v001.z);

	temp_min.x = fmin(temp_min.x, v010.x);
	temp_min.y = fmin(temp_min.y, v010.y);
	temp_min.z = fmin(temp_min.z, v010.z);

	temp_max.x = fmax(temp_max.x, v010.x);
	temp_max.y = fmax(temp_max.y, v010.y);
	temp_max.z = fmax(temp_max.z, v010.z);

	temp_min.x = fmin(temp_min.x, v011.x);
	temp_min.y = fmin(temp_min.y, v011.y);
	temp_min.z = fmin(temp_min.z, v011.z);

	temp_max.x = fmax(temp_max.x, v011.x);
	temp_max.y = fmax(temp_max.y, v011.y);
	temp_max.z = fmax(temp_max.z, v011.z);

	temp_min.x = fmin(temp_min.x, v100.x);
	temp_min.y = fmin(temp_min.y, v100.y);
	temp_min.z = fmin(temp_min.z, v100.z);

	temp_max.x = fmax(temp_max.x, v100.x);
	temp_max.y = fmax(temp_max.y, v100.y);
	temp_max.z = fmax(temp_max.z, v100.z);

	temp_min.x = fmin(temp_min.x, v101.x);
	temp_min.y = fmin(temp_min.y, v101.y);
	temp_min.z = fmin(temp_min.z, v101.z);

	temp_max.x = fmax(temp_max.x, v101.x);
	temp_max.y = fmax(temp_max.y, v101.y);
	temp_max.z = fmax(temp_max.z, v101.z);

	temp_min.x = fmin(temp_min.x, v110.x);
	temp_min.y = fmin(temp_min.y, v110.y);
	temp_min.z = fmin(temp_min.z, v110.z);

	temp_max.x = fmax(temp_max.x, v110.x);
	temp_max.y = fmax(temp_max.y, v110.y);
	temp_max.z = fmax(temp_max.z, v110.z);

	temp_min.x = fmin(temp_min.x, v111.x);
	temp_min.y = fmin(temp_min.y, v111.y);
	temp_min.z = fmin(temp_min.z, v111.z);

	temp_max.x = fmax(temp_max.x, v111.x);
	temp_max.y = fmax(temp_max.y, v111.y);
	temp_max.z = fmax(temp_max.z, v111.z);

	/*m_min = temp_min;
	m_max = temp_max;

	m_center = (m_max + m_min) * 0.5f;
	m_delta = m_max - m_min;*/

	return BBox(temp_min, temp_max);
}
