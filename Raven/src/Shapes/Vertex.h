#pragma once

#include "ravenpch.h"
#include "Math/glm/glm.hpp"

using glm::vec3;

// TODO: will be implemented for smooth shading

namespace raven
{
	class Vertex
	{
	public:
		Vertex(): m_position(vec3(0.0f)), m_normal(vec3(0.0f)), m_uv(vec3(0.0f)), m_num(0) {}
		Vertex(const vec3& pos) : m_position(pos), m_normal(vec3(0.0f)), m_uv(vec2(0.0f)), m_num(0) {}

	public:
		void add_vertex_normal(const vec3 &normal)
		{
			m_normal += normal;
			m_num++;
		}
		void add_vertex_normal(const vec3& normal, float area)
		{
			m_normal += normal * area;
			m_num++;
		}

		void normalize_vertex_normals()
		{
			m_normal = normalize(m_normal);
		}

	public:
		vec3 m_position;
		vec3 m_normal;
		vec2 m_uv; 
		int m_num;
	};
}
