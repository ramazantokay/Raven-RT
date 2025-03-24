#pragma once
#include "ravenpch.h"

inline float gaussian_filter(float x, float y, float sigma)
{
	return exp(-(x * x + y * y) / (2 * sigma * sigma)) / (2 * RAVEN_PI * sigma);
}

inline float beer_law(float c, float t)
{
	return exp(-c * t);
}

inline int get_min_element_index_in_glm_vec3(glm::vec3& v) // abs value sense
{
	glm::vec3 abs_vec = glm::abs(v);

	if (abs_vec.x <= abs_vec.y && abs_vec.x <= abs_vec.z) return 0;
	if (abs_vec.y <= abs_vec.z) return 1;
	
	return 2;
	
}