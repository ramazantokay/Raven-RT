#include "ravenpch.h"
#include "CheckboardTexture.h"



vec3 CheckboardTexture::get_texture_color(const vec2& uv) const
{
	return vec3();
}

vec3 CheckboardTexture::get_texture_color(const vec2& uv, const vec3& pos) const
{
	vec3 color(0.0f);

	int x = (int)((pos.x + m_offset) * m_scale) % 2;
	int y = (int)((pos.y + m_offset) * m_scale) % 2;
	int z = (int)((pos.z + m_offset) * m_scale) % 2;

	bool is_black = (x + y + z) % 2;
	if (is_black)
		color = m_black;
	else
		color = m_white;

	return color;
}

vec3 CheckboardTexture::get_normal_map_value(const vec2& uv, const mat3& tbn) const
{
	return tbn[2];
}

vec3 CheckboardTexture::get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const
{
    return vec3();
}

vec3 CheckboardTexture::get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv) const
{
    return normal;
}
