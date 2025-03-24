#pragma once
#include "ravenpch.h"

#include "Texture.h"

using raven::Texture;

class CheckboardTexture : public Texture
{
public:
	CheckboardTexture(const vec3& black, const vec3& white, float scale, float offset, const std::string& decal) :
		Texture(decal), m_black(black), m_white(white), m_offset(offset), m_scale(scale) {}

	vec3 get_texture_color(const vec2& uv) const override;
	vec3 get_texture_color(const vec2& uv, const vec3& pos) const override;
	vec3 get_normal_map_value(const vec2& uv, const mat3& tbn)const override;
	vec3 get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const override;
	vec3 get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv)const override;



public:

	vec3 m_white;
	vec3 m_black;
	float m_scale;
	float m_offset;
};