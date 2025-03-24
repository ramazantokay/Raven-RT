#include "ravenpch.h"
#include "PerlinTexture.h"

PerlinTexture::PerlinTexture(const std::string& decal, const std::string& type, float scale, float bumpfactor):
    Texture(decal), m_scaling_factor(scale), m_bump_factor(bumpfactor)
{
    m_noise_type = parse_noise_type(type);
    m_perlin_noise = raven::PerlinNoise();
}

vec3 PerlinTexture::get_texture_color(const vec2& uv) const
{
    return vec3();
}

vec3 PerlinTexture::get_texture_color(const vec2& uv, const vec3& pos) const
{
	float noise = get_perlin_value(pos);

	return vec3(noise, noise, noise);
}

vec3 PerlinTexture::get_normal_map_value(const vec2& uv, const mat3& tbn) const
{
    return vec3();
}

vec3 PerlinTexture::get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const
{
    return vec3();
}

vec3 PerlinTexture::get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv) const
{
	const float epsilon = 0.001;
	float noise = get_perlin_value(pos);

	// Calculate gradient at p.
	vec3 g = vec3(0.0f);
	g.x = ((get_perlin_value(vec3(pos.x + epsilon, pos.y, pos.z)) - noise) / epsilon) * m_bump_factor;
	g.y = ((get_perlin_value(vec3(pos.x, pos.y + epsilon, pos.z)) - noise) / epsilon) * m_bump_factor;
	g.z = ((get_perlin_value(vec3(pos.x, pos.y, pos.z + epsilon)) - noise) / epsilon) * m_bump_factor;

	vec3 g_Tangent = dot(g,normal) * normal;
	vec3 g_Projected = g - g_Tangent;
	vec3 bumped_normal = normalize(normal - g_Projected);

	return bumped_normal;
}

float PerlinTexture::get_perlin_value(const glm::vec3& pos) const
{
	glm::vec3 scaledPosition = pos * m_scaling_factor;
	float noise = m_perlin_noise.get_noise(scaledPosition);

	if (m_noise_type == raven::NoiseType::Abs)
	{
		noise = abs(noise);
	}
	if (m_noise_type == raven::NoiseType::Linear	)
	{
		noise = (noise + 1.0f) / 2.0f;
	}

	return noise;
}

raven::NoiseType PerlinTexture::parse_noise_type(const std::string& str)
{
	raven::NoiseType noise_type;

	if (str == "absval")
	{
		noise_type = raven::NoiseType::Abs;
	}
	else if (str == "linear")
	{
		noise_type = raven::NoiseType::Linear;
	}

	return noise_type;
}
