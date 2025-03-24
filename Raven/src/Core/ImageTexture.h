#pragma once

#include "ravenpch.h"
#include "Core/Image.h"
#include "Texture.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/component_wise.hpp>

namespace raven
{
	enum class InterpolationType
	{
		None,
		Nearest,
		Bilinear,
		Trilinear // TODO: will be implemented
	};


	class ImageTexture : public Texture
	{
	public:
		ImageTexture(const std::string& path, const std::string& interpolation, const std::string& decal, float normalizer, float bumpfactor);

		vec3 get_texture_color(const vec2& uv) const override;
		vec3 get_texture_color(const vec2& uv, const vec3& pos) const override;
		vec3 get_normal_map_value(const vec2& uv, const mat3& tbn) const override;
		vec3 get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const override;
		vec3 get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv) const override;

	public:
		vec3 texture_fetch(int u, int v) const;
		vec3 get_texture_normal(const vec2& uv) const;
		InterpolationType get_interpolation_type(const std::string& type);
	
	public:


	// Vars
	public:
		int m_width;
		int m_height;
		int m_components;
		float m_normalizer;
		float m_bump_factor;
		//unsigned char* m_texture;
		Image* m_texture;
		std::string m_texture_path;
		InterpolationType m_type;
	};

}