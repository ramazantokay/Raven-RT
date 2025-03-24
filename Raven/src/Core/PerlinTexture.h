#pragma once

#include "ravenpch.h"
#include "Texture.h"
#include "PerlinNoise.h"

using raven::Texture;

class PerlinTexture : public Texture
{
public:

	PerlinTexture(const std::string& decal, const std::string& type, float scale, float bumpfactor);
	vec3 get_texture_color(const vec2& uv) const override;
	vec3 get_texture_color(const vec2& uv, const vec3& pos) const override;
	vec3 get_normal_map_value(const vec2& uv, const mat3& tbn)const override;
	vec3 get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const override;
	vec3 get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv)const override;
	float get_perlin_value(const glm::vec3& pos) const;

	raven::NoiseType parse_noise_type(const std::string& str);

public:
	raven::NoiseType m_noise_type;
	float m_scaling_factor;
	float m_bump_factor;
	raven::PerlinNoise m_perlin_noise;



};