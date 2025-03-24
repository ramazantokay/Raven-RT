#pragma once

#include "Math/glm/glm.hpp"
#include <ostream>

using glm::vec3;

namespace raven
{
	class Ray
	{
	public:
		Ray() :
			m_origin(vec3(0.0f)), m_direction(vec3(0.0f)), m_is_in_medium(false), m_t(0.0f), m_motion_time(0.0f), m_is_direct(true){}
		Ray(const vec3& origin, const vec3& direction) : 
			m_origin(origin), m_direction(direction), m_is_in_medium(false), m_t(0.0f), m_motion_time(0.0f), m_is_direct(true) {}
		Ray(const vec3& ray) { *this = ray; }

		vec3 origin() const { return m_origin; }
		vec3 direction() const { return (m_direction); }
		vec3 at(float t) const { return m_origin + (t * m_direction); }
		float time() const { return m_t; }

	public:
		vec3 m_origin;
		vec3 m_direction;
		bool m_is_in_medium;
		float m_t; // TODO:
		float m_motion_time;
		bool m_is_direct;

		// TODO: add m_inv_direction
	};

	inline std::ostream& operator<<(std::ostream& out, const Ray& ray)
	{
		return out << "Origin: " << ray.origin() << "Direction: " << normalize(ray.direction()) << "\n";
	}
}
