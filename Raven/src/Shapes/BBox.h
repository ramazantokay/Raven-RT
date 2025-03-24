#pragma once

#include "Core/Ray.h"
#include "Math/glm/glm.hpp"
#include "Math/Transformations.h"
#include "ravenpch.h"

// TODO: in Ray class, we can pre-calculate inverse direction in a different array, so that we can avoid division in intersect function
// Do a benchmarking test, after finishing hw2 

using glm::vec3;

namespace raven
{
	class BBox
	{
	public:
		// Constructors
		BBox() :m_min(vec3(raven_infinity)), m_max(vec3(-raven_infinity)), m_center(vec3(0.0f)), m_delta(vec3(0.0f)) {}
		BBox(const vec3& _min, const vec3& _max) : m_min(_min), m_max(_max), m_center((_max + _min) * 0.5f), m_delta(_max - _min) {}

		// Helper functions
		void update_bounds(const BBox& bbox);
		float intersect(const Ray& ray) const;
		
		BBox apply_transformation(Transformations& transformation);

	public:
		vec3 m_min;
		vec3 m_max;
		vec3 m_center;
		vec3 m_delta;
	};
}