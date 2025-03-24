#include "ravenpch.h"
#include "ImageTexture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"

raven::ImageTexture::ImageTexture(const std::string& path, const std::string& interpolation, const std::string& decal, float normalizer, float bumpfactor):
	m_texture_path(path), Texture(decal), m_normalizer(normalizer), m_bump_factor(bumpfactor)
{
    m_type = get_interpolation_type(interpolation);

    // Loading texture
    //m_texture = stbi_load(m_texture_path.c_str(), &m_width, &m_height, &m_components, 3);

    m_texture = new Image(m_texture_path);
	m_width = m_texture->m_width;
	m_height = m_texture->m_height;
}


vec3 raven::ImageTexture::get_texture_color(const vec2& uv) const
{
    vec3 texel(0.0f);

    float _u = uv.x * m_width;
    float _v = uv.y * m_height;

    if (m_type == InterpolationType::Nearest)
    {
        texel = texture_fetch(int(_u), int(_v));
    }
    else if (m_type == InterpolationType::Bilinear)
    {
        int p = int(floor(_u));
        int q = int(floor(_v));

        float dx = _u - p;
        float dy = _v - q;

        texel = texture_fetch(p, q) * (1 - dx) * (1 - dy) +
            texture_fetch(p + 1, q) * dx * (1 - dy) +
            texture_fetch(p, q + 1) * (1 - dx) * dy +
			texture_fetch(p + 1, q + 1) * dx * dy;
    }
    else if (m_type == InterpolationType::Trilinear)
    {
        // TODO:
    }

    if (m_decal_mode != DecalMode::ReplaceBackground)
    {
        texel = texel / m_normalizer;
    }

    return texel;

}

vec3 raven::ImageTexture::get_texture_color(const vec2& uv, const vec3& pos) const
{
	return vec3();
}

vec3 raven::ImageTexture::get_normal_map_value(const vec2& uv, const mat3& tbn) const
{
    vec3 texture_normal = get_texture_normal(uv);
    vec3 tbn_normal = normalize(tbn * texture_normal);

    return tbn_normal;
}

vec3 raven::ImageTexture::get_bump_map_value(const vec3& normal, const mat3& tbn, const vec2& uv) const
{
    vec3 _u = tbn[0];
    vec3 _v = tbn[1];

    int i = int(floor(uv.x * m_width));
    int j = int(floor(uv.y * m_height));

    vec3 t_f1 = texture_fetch(i + 1, j);
    vec3 t_f2 = texture_fetch(i , j);
    vec3 t_f3 = texture_fetch(i , j+1);

    float f1 = glm::compAdd(t_f1) / 3.0f;
    float f2 = glm::compAdd(t_f2) / 3.0f;
	float f3 = glm::compAdd(t_f3) / 3.0f;

    float du = (f1 - f2) * m_bump_factor;
    float dv = (f3 - f2) * m_bump_factor;

    vec3 qu = _u + du * normal;
    vec3 qv = _v + dv * normal;

    vec3 b_normal = normalize(cross(qv, qu));

	if (dot(b_normal, normal) < 0)
    {
        b_normal = -1.0f * b_normal;
    }

    return b_normal;
}

vec3 raven::ImageTexture::get_bump_map_value(const vec3& pos, const vec3& normal, const mat3& tbn, const vec2& uv) const
{
    return vec3(0.0f);
}

vec3 raven::ImageTexture::texture_fetch(int u, int v) const
{
    u = (u % m_width + m_width) % m_width;
    v = (v % m_height + m_height) % m_height;

    return m_texture->get_pixel(u, v);
}

vec3 raven::ImageTexture::get_texture_normal(const vec2& uv) const
{
    vec3 _rgb = get_texture_color(uv);
    vec3 _normal = normalize(_rgb - vec3(0.5f));

    return _normal;
}

raven::InterpolationType raven::ImageTexture::get_interpolation_type(const std::string& type)
{
    // TODO: remove temp var
    InterpolationType temp_type = raven::InterpolationType::None;

    if (type == "nearest")
        temp_type = InterpolationType::Nearest;
    else if (type == "bilinear")
        temp_type = InterpolationType::Bilinear;
    else if (type == "trilinear")
        temp_type = InterpolationType::Trilinear;

    return temp_type;
}
