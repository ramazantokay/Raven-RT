#include "ravenpch.h"
#include "Sphere.h"

Sphere::Sphere(const vec3& _center, float _radius, const rgb& _color) : m_center(_center), m_radius(_radius), m_color(_color)
{
}

Sphere::Sphere(const vec3& _center, float _radius, const int& _mat) : m_center(_center), m_radius(_radius), m_material_id(_mat)
{
    vec3 temp_delta(_radius);
	m_bbox = BBox(_center - temp_delta, _center + temp_delta);
}

Sphere::Sphere(const vec3& _center, float _radius, const int& _mat, raven::Texture* texture, raven::Texture* normal, raven::Transformations transformation) :
    m_center(_center), m_radius(_radius), m_material_id(_mat), m_texture(texture),m_normal_texture(normal), m_transformation(transformation)
{
	vec3 temp_delta(_radius);
	m_bbox = BBox(_center - temp_delta, _center + temp_delta);

    if (transformation.m_transformations != glm::mat4(1.0f))
    {
        m_bbox = m_bbox.apply_transformation(transformation);
    }
	m_specular_texture = nullptr;
	m_texture_id = -1;
}

bool Sphere::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{

	if (m_transformation.m_transformations == glm::mat4(1.0f))
	{
		vec3 temp_dir = ray.origin() - m_center;
		float a = glm::length2(ray.direction());
		float half_b = dot(temp_dir, ray.direction());
		float c = glm::length2(temp_dir) - m_radius * m_radius;

		float discriminant = half_b * half_b - a * c;

		if (discriminant < 0.f)
		{
			rec.m_t = std::numeric_limits<float>::infinity();
			rec.m_shape = NULL;
			return false;
		}

		float discriminant_sqrt = sqrt(discriminant);
		float root = (-half_b - discriminant_sqrt) / a;

		if (root < t_min || t_max < root)
		{
			root = (-half_b + discriminant_sqrt) / a;

			if (root < t_min || t_max < root)
			{
				rec.m_t = std::numeric_limits<float>::infinity();
				rec.m_shape = NULL;
				return false;
			}
		}

		rec.m_t = root;
		rec.m_normal = normalize((ray.at(rec.m_t) - m_center) / m_radius);
		rec.m_shape = this;
		rec.m_texture = this->m_texture;
		vec3 p = ray.at(rec.m_t) - m_center;

		rec.m_uv = get_uv(p, m_texture);

		if (m_normal_texture && m_normal_texture->m_is_normal_map)
		{

			vec2 uv = get_uv(p, m_normal_texture);
			mat3 tbn = calculate_tbn(p, rec.m_normal);
			rec.m_normal = m_normal_texture->get_normal_map_value(uv, tbn);

		}

		if (m_normal_texture && m_normal_texture->m_is_bump_map)
		{

			vec2 uv = get_uv(p, m_normal_texture);
			mat3 tbn = calculate_tbn(p, rec.m_normal);
			rec.m_normal = m_normal_texture->get_bump_map_value(rec.m_normal, tbn, uv);

		}

		return true;
	}
	else
	{
		glm::vec3 new_origin = glm::mat4(m_transformation.m_inverse_transformations) * glm::vec4(ray.m_origin,1.0f);
		glm::vec3 new_dir = (glm::mat3(m_transformation.m_inverse_transformations) * glm::vec4(ray.m_direction,0.0f));

		Ray local_ray(new_origin, new_dir);


		vec3 temp_dir = local_ray.origin() - m_center;
		float a = glm::length2(local_ray.direction());
		float half_b = dot(temp_dir, local_ray.direction());
		float c = glm::length2(temp_dir) - m_radius * m_radius;

		float discriminant = half_b * half_b - a * c;

		if (discriminant < 0.f)
		{
			rec.m_t = std::numeric_limits<float>::infinity();
			rec.m_shape = NULL;
			return false;
		}

		float discriminant_sqrt = sqrt(discriminant);
		float root = (-half_b - discriminant_sqrt) / a;

		if (root < t_min || t_max < root)
		{
			root = (-half_b + discriminant_sqrt) / a;

			if (root < t_min || t_max < root)
			{
				rec.m_t = std::numeric_limits<float>::infinity();
				rec.m_shape = NULL;
				return false;
			}
		}

		rec.m_t = root;
		//rec.m_normal = normalize(glm::vec3( m_transformation.m_normal_transformations * (glm::vec4((local_ray.at(rec.m_t) - m_center), 1.0f))) );
		rec.m_normal = normalize((local_ray.at(rec.m_t) - m_center));

		rec.m_shape = this;
		rec.m_texture = this->m_texture;

		vec3 p = local_ray.at(rec.m_t) - m_center;

		rec.m_uv = get_uv(p, m_texture);

		if (m_normal_texture && m_normal_texture->m_is_normal_map)
		{

			vec2 uv = get_uv(p, m_normal_texture);
			mat3 tbn = calculate_tbn(p, rec.m_normal);
			rec.m_normal = m_normal_texture->get_normal_map_value(uv, tbn);

		}

		if (m_normal_texture && m_normal_texture->m_is_bump_map)
		{

			vec2 uv = get_uv(p, m_normal_texture);
			mat3 tbn = calculate_tbn(p, rec.m_normal);
			rec.m_normal = m_normal_texture->get_bump_map_value(rec.m_normal, tbn, uv);

		}

		rec.m_normal = normalize(glm::vec3( m_transformation.m_normal_transformations * (glm::vec4(rec.m_normal, 0.0f))) );



		return true;
	}
}

bool Sphere::shadow_hit(const Ray& ray, float t_min, float t_max, float time) const
{
    return false;
}

int Sphere::get_material_id() const
{
    return m_material_id;
}

int Sphere::get_texture_id() const
{
	return m_texture_id;
}

const BBox& Sphere::get_bbox() const
{
	return m_bbox;
}

vec2 Sphere::get_uv(const vec3& p, raven::Texture* texture) const
{
	vec2 uv(0.0f);

	if (!texture)
		return uv;

	float theta = acos(p.y / m_radius);
	float phi = atan2(p.z, p.x);

	uv.x = (RAVEN_PI - phi) / (2.0f * RAVEN_PI);
	uv.y = theta / RAVEN_PI;

	return uv;
}

mat3 Sphere::calculate_tbn(vec3 p,  vec3 normal) const
{
	mat3 tbn(0.0f);

	if (!m_normal_texture)
		return tbn;

	float theta = acos(p.y / m_radius);
	float phi = atan2(p.z, p.x);

	vec3 T(2.0 * RAVEN_PI * p.z, 0.0f, -2.0f * RAVEN_PI * p.x);

	vec3 B(p.y * cos(phi) * RAVEN_PI, -1.0f*m_radius * sin(theta)*RAVEN_PI, p.y * sin(phi) * RAVEN_PI);

	if (m_normal_texture->m_is_normal_map)
	{
		T = normalize(T);
		B = normalize(B);
	}

	tbn = mat3(T, B, normal);

	return tbn;
}
