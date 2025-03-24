#include "ravenpch.h"
#include "MeshLight.h"
//
#include "Shapes/Triangle.h"

thread_local float raven::MeshLight::m_pw;
thread_local vec3 raven::MeshLight::m_position_on_light;
thread_local vec3 raven::MeshLight::m_wi;

raven::MeshLight::MeshLight(int material_id, raven::Texture* tex, raven::Texture* normal,
	std::vector<Shape*>& triangles, raven::Transformations transformation, const vec3& radiance)
	: Mesh(material_id, tex, normal, triangles, transformation), m_radiance(radiance)
{
	calculate_cdf();

	m_mesh_light_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	m_mesh_light_distribution = std::uniform_real_distribution<float>(0.0f, 1.0f);
}

vec3 raven::MeshLight::calculate_wi(const vec3& intersection_point, const vec3& normal)
{
	// Choose a random triangle
	float random = m_mesh_light_distribution(m_mesh_light_generator);

	int index = 0;
	for (int i = 0; i < m_cdf.size(); i++)
	{
		if (random <= m_cdf[i])
		{
			index = i;
			break;
		}
	}

	// Cast to triangle
	Triangle* tri = dynamic_cast<Triangle*>(m_triangless[index]);

	// Calculate the area of the triangle
	vec3 v0 = glm::vec3{ glm::mat4(m_transformation.m_transformations) * glm::vec4(tri->m_p0, 1.0f) };
	vec3 v1 = glm::vec3{ glm::mat4(m_transformation.m_transformations) * glm::vec4(tri->m_p1, 1.0f) };
	vec3 v2 = glm::vec3{ glm::mat4(m_transformation.m_transformations) * glm::vec4(tri->m_p2, 1.0f) };

	vec3 e1 = v1 - v0;
	vec3 e2 = v2 - v0;

	vec3 tri_normal = glm::normalize(glm::cross(e1, e2));

	float random_u = m_mesh_light_distribution(m_mesh_light_generator);
	float random_v = m_mesh_light_distribution(m_mesh_light_generator);

	vec3 pos = (1.0f - random_v) * v1 + (random_v * v2); // Found the problem, parantheses problem
	m_position_on_light = sqrt(random_u) * pos + (1.0f - sqrt(random_u)) * v0;

	float r_2 = glm::length2(m_position_on_light - intersection_point);
	float cos_theta_max = std::max(0.001f, glm::dot(tri_normal, (intersection_point - m_position_on_light)));
	m_pw = r_2 / (cos_theta_max * m_total_area);

	m_wi = glm::normalize(m_position_on_light - intersection_point);

	return m_wi;
}

float raven::MeshLight::calculate_distance(const vec3 intersection_point)
{
	return glm::length((m_position_on_light - m_wi * 0.001f) - intersection_point);
}

vec3 raven::MeshLight::calculate_E(const vec3& intersection_point)
{
	return m_radiance / m_pw;
}

bool raven::MeshLight::hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const
{
	bool res = Mesh::hit(ray, t_min, t_max, rec);
	rec.m_radiance = m_radiance;
	rec.m_is_light_shape = true;
	rec.m_light = (Light*)this;

	return res;
}

void raven::MeshLight::calculate_cdf()
{
	// Calculate the total area of the mesh
	m_total_area = 0.0f;

	for (Shape* triangle : m_triangless)
	{
		// Cast to triangle
		Triangle* tri = dynamic_cast<Triangle*>(triangle);
		
		// Calculate the area of the triangle

		vec3 v0 = glm::vec3{ glm::mat4(m_transformation.m_transformations) * vec4(tri->m_p0, 1.0f) };
		vec3 v1 = glm::vec3{ glm::mat4(m_transformation.m_transformations) * vec4(tri->m_p1, 1.0f) };
		vec3 v2 = glm::vec3{ glm::mat4(m_transformation.m_transformations) * vec4(tri->m_p2, 1.0f) };

		vec3 e1 = v1 - v0;
		vec3 e2 = v2 - v0;

		vec3 cross = glm::cross(e1, e2);

		float area = 0.5f * glm::length(cross);
		
		m_total_area += area;
		m_cdf.push_back(m_total_area);
	}

	// Normalize the cdf

	for (float& area : m_cdf)
	{
		area /= m_total_area;
	}


}
