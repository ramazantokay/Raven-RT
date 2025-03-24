#include "ravenpch.h"
#include "Triangle.h"

Triangle::Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const rgb& color) :  // TODO: will be removed or modifed
	m_p0(p0), m_p1(p1), m_p2(p2), m_surface_normal(vec3(0.0f)), m_color(color)
{
}

Triangle::Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const vec2& uv0, const vec2& uv1, const vec2& uv2, 
	const ShadingMode shadingmode, const int& mat, raven::Texture* tex, raven::Texture* normal, raven::Transformations transformation):
	m_p0(p0), m_p1(p1), m_p2(p2), m_uv0(uv0), m_uv1(uv1), m_uv2(uv2), m_shading_mode(shadingmode),
	m_mat_id(mat), m_texture(tex), m_normal_texture(normal), m_texture_id(-1), m_transformation(transformation)
{
	// Calculating Normal
	m_surface_normal = normalize(cross((m_p1 - m_p0), (m_p2 - m_p0)));

	// Calculating BBox
	vec3 temp_min = p0;
	vec3 temp_max = p0;

	temp_min.x = fmin(temp_min.x, p1.x);
	temp_min.y = fmin(temp_min.y, p1.y);
	temp_min.z = fmin(temp_min.z, p1.z);

	temp_max.x = fmax(temp_max.x, p1.x);
	temp_max.y = fmax(temp_max.y, p1.y);
	temp_max.z = fmax(temp_max.z, p1.z);
	
	
	temp_min.x = fmin(temp_min.x, p2.x);
	temp_min.y = fmin(temp_min.y, p2.y);
	temp_min.z = fmin(temp_min.z, p2.z);

	temp_max.x = fmax(temp_max.x, p2.x);
	temp_max.y = fmax(temp_max.y, p2.y);
	temp_max.z = fmax(temp_max.z, p2.z);

	m_bbox = BBox(temp_min, temp_max);

	m_tbn = calculate_tbn();

	m_specular_texture = nullptr;
}

Triangle::Triangle(const Vertex& p0, const Vertex& p1, const Vertex& p2, const ShadingMode shadingmode, 
	const int& mat, raven::Texture* tex, raven::Texture* normal, raven::Transformations transformation):
	m_p0(p0.m_position), m_p1(p1.m_position), m_p2(p2.m_position), m_uv0(p0.m_uv), m_uv1(p1.m_uv), m_uv2(p2.m_uv), m_shading_mode(shadingmode),
	m_mat_id(mat), m_texture(tex), m_normal_texture(normal), m_texture_id(-1), m_transformation(transformation)
{
	//m_vertices[0] = p0;
	//m_vertices[1] = p1;
	//m_vertices[2] = p2;

	// Calculating Normal
	m_surface_normal = normalize(cross((m_p1 - m_p0), (m_p2 - m_p0)));

	// Calculating BBox
	vec3 temp_min = m_p0;
	vec3 temp_max = m_p0;

	temp_min.x = fmin(temp_min.x, m_p1.x);
	temp_min.y = fmin(temp_min.y, m_p1.y);
	temp_min.z = fmin(temp_min.z, m_p1.z);

	temp_max.x = fmax(temp_max.x, m_p1.x);
	temp_max.y = fmax(temp_max.y, m_p1.y);
	temp_max.z = fmax(temp_max.z, m_p1.z);


	temp_min.x = fmin(temp_min.x, m_p2.x);
	temp_min.y = fmin(temp_min.y, m_p2.y);
	temp_min.z = fmin(temp_min.z, m_p2.z);

	temp_max.x = fmax(temp_max.x, m_p2.x);
	temp_max.y = fmax(temp_max.y, m_p2.y);
	temp_max.z = fmax(temp_max.z, m_p2.z);

	m_bbox = BBox(temp_min, temp_max);

	m_tbn = calculate_tbn();

	m_specular_texture = nullptr;
}


Triangle::Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const int& mat, const int& tex_id) :
	m_p0(p0), m_p1(p1), m_p2(p2), m_uv0(vec2(0.f)), m_uv1(vec2(0.f)), m_uv2(vec2(0.f)), m_mat_id(mat), m_texture_id(tex_id)

{
	// Calculating Normal
	m_surface_normal = normalize(cross((m_p1 - m_p0), (m_p2 - m_p0)));

	// Calculating BBox
	vec3 temp_min = p0;
	vec3 temp_max = p0;

	temp_min.x = fmin(temp_min.x, p1.x);
	temp_min.y = fmin(temp_min.y, p1.y);
	temp_min.z = fmin(temp_min.z, p1.z);

	temp_max.x = fmax(temp_max.x, p1.x);
	temp_max.y = fmax(temp_max.y, p1.y);
	temp_max.z = fmax(temp_max.z, p1.z);


	temp_min.x = fmin(temp_min.x, p2.x);
	temp_min.y = fmin(temp_min.y, p2.y);
	temp_min.z = fmin(temp_min.z, p2.z);

	temp_max.x = fmax(temp_max.x, p2.x);
	temp_max.y = fmax(temp_max.y, p2.y);
	temp_max.z = fmax(temp_max.z, p2.z);

	m_bbox = BBox(temp_min, temp_max);
}

Triangle::Triangle(const int v0, const int v1, const int v2, const ShadingMode shadingmode, 
	const int& mat, raven::Texture* tex, raven::Texture* normal, raven::Transformations transformation):
	m_v0_id(v0), m_v1_id(v1), m_v2_id(v2), m_shading_mode(shadingmode), m_mat_id(mat),
	m_texture(tex), m_normal_texture(normal), m_texture_id(-1), m_transformation(transformation)

{
	// get vertices 

	vec3 p0 = raven_scene->m_vertices[v0].m_position;
	vec3 p1 = raven_scene->m_vertices[v1].m_position;
	vec3 p2 = raven_scene->m_vertices[v2].m_position;
	

	// TODO: Refactor it
	m_p0 = p0;
	m_p1 = p1;
	m_p2 = p2;

	// uvs
	// TODO: Refactor it

	m_uv0 = raven_scene->m_vertices[v0].m_uv;
	m_uv1 = raven_scene->m_vertices[v1].m_uv;
	m_uv2 = raven_scene->m_vertices[v2].m_uv;

	//if (length(p1-p0) <= 0.0f || length(p2-p0) <= 0.0f ||length(cross((p1 - p0), (p2 - p0))) <= 0.0f)
	//{
	//	m_surface_normal = vec3(0.0f, 0.0f, 0.0f);
	//}
	//else
	//{

	//// calculate normal

	//}
	m_surface_normal = normalize(cross((p1 - p0), (p2 - p0)));

	m_surface_area = length(m_surface_normal) / 2.0f;

	// calculate BBox

	vec3 temp_min = p0;
	vec3 temp_max = p0;

	temp_min.x = fmin(temp_min.x, p1.x);
	temp_min.y = fmin(temp_min.y, p1.y);
	temp_min.z = fmin(temp_min.z, p1.z);

	temp_max.x = fmax(temp_max.x, p1.x);
	temp_max.y = fmax(temp_max.y, p1.y);
	temp_max.z = fmax(temp_max.z, p1.z);


	temp_min.x = fmin(temp_min.x, p2.x);
	temp_min.y = fmin(temp_min.y, p2.y);
	temp_min.z = fmin(temp_min.z, p2.z);

	temp_max.x = fmax(temp_max.x, p2.x);
	temp_max.y = fmax(temp_max.y, p2.y);
	temp_max.z = fmax(temp_max.z, p2.z);

	m_bbox = BBox(temp_min, temp_max);

	m_tbn = calculate_tbn();

	m_specular_texture = nullptr;

}

// Deprecated
Triangle::Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const int& mat, raven::Transformations transformation):
	m_p0(p0), m_p1(p1), m_p2(p2), m_mat_id(mat), m_transformation(transformation)
{
	// Calculating Normal
	m_surface_normal = normalize(cross((m_p1 - m_p0), (m_p2 - m_p0)));

	// Calculating BBox
	vec3 temp_min = p0;
	vec3 temp_max = p0;

	temp_min.x = fmin(temp_min.x, p1.x);
	temp_min.y = fmin(temp_min.y, p1.y);
	temp_min.z = fmin(temp_min.z, p1.z);

	temp_max.x = fmax(temp_max.x, p1.x);
	temp_max.y = fmax(temp_max.y, p1.y);
	temp_max.z = fmax(temp_max.z, p1.z);


	temp_min.x = fmin(temp_min.x, p2.x);
	temp_min.y = fmin(temp_min.y, p2.y);
	temp_min.z = fmin(temp_min.z, p2.z);

	temp_max.x = fmax(temp_max.x, p2.x);
	temp_max.y = fmax(temp_max.y, p2.y);
	temp_max.z = fmax(temp_max.z, p2.z);

	m_bbox = BBox(temp_min, temp_max);

	if (m_transformation.m_transformations != glm::mat4(1.0f)) // is not identity
	{
		m_bbox = m_bbox.apply_transformation(transformation);
	}
	

}

bool Triangle::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const // TODO: implement Moller Intersection
{

	vec3 col1 = (raven_scene->m_vertices[m_v0_id].m_position) - (raven_scene->m_vertices[m_v1_id].m_position);
	vec3 col2 = (raven_scene->m_vertices[m_v0_id].m_position) - (raven_scene->m_vertices[m_v2_id].m_position);

	vec3 col3 = ray.m_direction;

	const float detA = det(col1, col2, col3);

	if (detA == 0.0f)
		return false;

	const vec3 b = (raven_scene->m_vertices[m_v0_id].m_position - ray.m_origin) / detA;

	const float beta = det(b, col2, col3);

	if (beta < 0.0f - t_min || beta > 1.0f + t_min)
		return false;

	const float gamma = det(col1, b, col3);

	if (gamma < 0.0f - t_min || beta + gamma > 1.0f + t_min)
		return false;

	const float t_val = det(col1, col2, b);

	if (t_val > t_min)
	{
		rec.m_t = t_val;
		rec.m_shape = this;
		

		if (m_shading_mode == ShadingMode::Flat)
		{
			rec.m_normal = this->m_surface_normal;
		}
		// TODO: will add smooth shading
		else if (m_shading_mode == ShadingMode::Smooth)
		{
			//rec.m_normal = this->m_surface_normal;

			rec.m_normal = normalize( 
				 (1.0f - beta - gamma) * raven_scene->m_vertices[m_v0_id].m_normal +
								beta * raven_scene->m_vertices[m_v1_id].m_normal +
								gamma * raven_scene->m_vertices[m_v2_id].m_normal)
				;

			//rec.m_normal = ( n1 + n2 + n3);
			//rec.m_normal = this->m_surface_normal;

		}
		
		rec.m_uv = m_uv0 + beta * (m_uv1 - m_uv0) + gamma * (m_uv2 - m_uv0);
		rec.m_texture = this->m_texture;

		 //Normal Mapping
		if (m_normal_texture !=NULL)
		{
			if (m_normal_texture->m_is_normal_map)
				rec.m_normal = m_normal_texture->get_normal_map_value(rec.m_uv, m_tbn);
		}
		// Bump Mapping
		if (m_normal_texture != NULL)
		{
			if (m_normal_texture->m_is_bump_map)
				rec.m_normal = m_normal_texture->get_bump_map_value(rec.m_normal, m_tbn, rec.m_uv);
			//rec.m_normal = normalize(glm::vec3(m_transformation.m_normal_transformations * (glm::vec4(rec.m_normal, 0.0f))));
		}

		// Specular Mapping

		// TODO:
		
		
		return true;
	}
	return false;

}

bool Triangle::shadow_hit(const Ray& ray, float t_min, float t_max, float time) const
{
	vec3 col1 = m_p0 - m_p1;
	vec3 col2 = m_p0 - m_p2;
	vec3 col3 = ray.direction();

	const float detA = det(col1, col2, col3);

	if (detA == 0.0f)
		return false;

	const vec3 b = (m_p0 - ray.origin()) / detA;

	const float beta = det(b, col2, col3);

	if (beta < 0.0f - t_min || beta > 1.0f + t_min)
		return false;

	const float gamma = det(col1, b, col3);

	if (gamma < 0.0f - t_min || beta + gamma > 1.0f + t_min)
		return false;

	const float t_val = det(col1, col2, b);

	return (t_val >= t_min && t_val <= t_max);
}


int Triangle::get_material_id() const
{
	return m_mat_id;
}

int Triangle::get_texture_id() const
{
	return m_texture_id;
}

const BBox& Triangle::get_bbox() const
{
	return m_bbox;
}

vec2 Triangle::get_uv(float beta, float gamma, raven::Texture* tex) const
{
	vec2 uv(0.0f);

	if (!tex)
	{
		return uv;
	}

	uv = m_uv0 + beta * (m_uv1 - m_uv0) + gamma * (m_uv2 - m_uv0);

	return uv;
}

mat3 Triangle::calculate_tbn()
{
	// TODO: refactor it
	mat3 temp_tbn = mat3(0.0f);

	if (!m_normal_texture)
	{
		return m_tbn;
	}

	vec3 e1 = m_p1 - m_p0;
	vec3 e2 = m_p2 - m_p0;

	float a = m_uv1.x - m_uv0.x;
	float b = m_uv1.y - m_uv0.y;
	float c = m_uv2.x - m_uv0.x;
	float d = m_uv2.y - m_uv0.y;

	float detA = d * a - b * c;

	float invA[2][2];
	invA[0][0] = d / detA;
	invA[0][1] = -1 * b / detA;
	invA[1][0] = -1 * c / detA;
	invA[1][1] = a / detA;

	vec3 T = e1 * invA[0][0] + e2 * invA[0][1];
	vec3 B = e1 * invA[1][0] + e2 * invA[1][1];

	temp_tbn = mat3(T, B, m_surface_normal);


	return temp_tbn;
}
