#pragma once

#include "ravenpch.h"

#include "Math/glm/glm.hpp"

using glm::vec2;
using glm::vec3;
using glm::mat3;

namespace raven
{

	enum class DecalMode
	{
		None,
		ReplaceKd,
		BlendKd,
		ReplaceNormal,
		ReplaceBackground,
		ReplaceAll,
		BumpNormal
	};

	class Texture
	{
	// Constructor
	public:
		Texture(const std::string& decalmode) 
		{
			m_decal_mode = get_decalmode(decalmode);

			m_is_normal_map = false;
			if (m_decal_mode == DecalMode::ReplaceNormal) { m_is_normal_map = true; }

			m_is_bump_map = false;
			if (m_decal_mode == DecalMode::BumpNormal) { m_is_bump_map = true; }
		}
	
	// Virtual functions
	public:
		virtual vec3 get_texture_color(const vec2& uv) const = 0;
		virtual vec3 get_texture_color(const vec2& uv, const vec3& pos) const = 0;
		virtual vec3 get_normal_map_value(const vec2&uv, const mat3& tbn) const = 0;
		virtual vec3 get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const = 0;
		virtual vec3 get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv) const = 0;
	
	// Helpers
	public:
		DecalMode get_decal_value() const { return m_decal_mode; }
		DecalMode get_decalmode(const std::string& mode) const
		{
			DecalMode temp_decal = raven::DecalMode::None;
			// TODO: use unordered map
			if (mode == "replace_kd")
				temp_decal = DecalMode::ReplaceKd;
			else if (mode == "blend_kd")
				temp_decal = DecalMode::BlendKd;
			else if (mode == "replace_normal")
				temp_decal = DecalMode::ReplaceNormal;
			else if (mode == "replace_background")
				temp_decal = DecalMode::ReplaceBackground;
			else if (mode == "replace_all")
				temp_decal = DecalMode::ReplaceAll;
			else if (mode == "bump_normal")
				temp_decal = DecalMode::BumpNormal;
			
			return temp_decal;
		}

	// Vars
	public:
		bool m_is_normal_map;
		bool m_is_bump_map;
		//bool m_is_specular_map;
		DecalMode m_decal_mode;
	};
}