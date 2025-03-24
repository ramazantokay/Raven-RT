#pragma once

#include "Ray.h"
#include "Math/glm/glm.hpp"
#include "RGB.h"
#include "Core/Texture.h"
#include "Lights/Light.h"

class Shape;

using glm::vec2;
using glm::vec3;
using raven::rgb;

class HitRecord
{
public:
	HitRecord() : m_shape(NULL), m_normal(vec3(0.0)), m_color(0.0f, 0.0f, 0.0f), m_t(0.0f), 
		m_texture(NULL), m_is_light_shape(false), m_radiance(0.0f), m_uv(0.0f){}
public:
	const Shape* m_shape;
	raven::Texture* m_texture;
	vec2 m_uv;
	vec3 m_normal;
	rgb m_color;
	float m_t;
	vec3 m_radiance;
	bool m_is_light_shape;
	Light* m_light;

};