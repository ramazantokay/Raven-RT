#pragma once

#include "Core/Ray.h"
#define GLM_ENABLE_EXPERIMENTAL

#include "Math/glm/glm.hpp"
#include "Math/glm/ext.hpp"
#include "Math/glm/gtx/norm.hpp"
#include "Math/glm/gtc/matrix_access.hpp"
#include "Core/Material.h"
#include "Core/RGB.h"
#include "Shape.h"
#include "Shapes/Vertex.h"
#include "Math/Transformations.h"
#include "Core/Texture.h"
#include "Utils/Scene.h"
#include "Utils/RavenGlobals.h"

using glm::vec3;
using glm::mat3;
using raven::Material;
using raven::Vertex;

enum ShadingMode
{
	None, 
	Flat,
	Smooth
};

class Triangle : public Shape
{
public:
	// Constructors

	Triangle() = default;
	Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const rgb& color);
	Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const int& mat, const int& tex_id);
	Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const vec2& uv0, const vec2& uv1,
		const vec2& uv2, const ShadingMode shadingmode, const int& mat, raven::Texture *tex, raven::Texture* normal, raven::Transformations transformation);
	
	Triangle(const raven::Vertex& p0, const raven::Vertex& p1, const raven::Vertex& p2, 
		const ShadingMode shadingmode, const int& mat, raven::Texture *tex, raven::Texture* normal, raven::Transformations transformation);
	
	
	
	Triangle(const int v0, const int v1, const int v2, 
		const ShadingMode shadingmode, const int& mat,
		raven::Texture *tex, raven::Texture* normal, raven::Transformations transformation);


	Triangle(const vec3& p0, const vec3& p1, const vec3& p2, const int& mat, raven::Transformations transformation); // deprecated

	// Virtual Functions

	bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;
	bool shadow_hit(const Ray& ray, float t_min, float t_max, float time) const override;
	int get_material_id() const override;
	int get_texture_id() const override;
	const BBox& get_bbox() const override;

	// Helper functions

	inline float det(const vec3& v0, const vec3& v1, const vec3& v2) const
	{
		return v0.x * (v1.y * v2.z - v2.y * v1.z) +
			v1.x * (v2.y * v0.z - v0.y * v2.z) +
			v2.x * (v0.y * v1.z - v1.y * v0.z);
	}

	vec2 get_uv(float beta, float gamma, raven::Texture* tex) const;

	mat3 calculate_tbn();

	// Variables
public:
	vec3 m_p0;
	vec3 m_p1;
	vec3 m_p2;
	vec2 m_uv0; // uv for p0
	vec2 m_uv1; // uv for p1
	vec2 m_uv2; // uv for p2

	//Vertex m_vertices[3];

	int m_v0_id;
	int m_v1_id;
	int m_v2_id;



	vec3 m_surface_normal;
	float m_surface_area;
	rgb m_color;
	int m_mat_id;
	int m_texture_id;
	BBox m_bbox;
	raven::Texture* m_texture; // Add this to Shape class
	raven::Texture* m_normal_texture; // Add this to Shape class
	raven::Texture* m_specular_texture; // Add this to Shape class
	raven::Transformations m_transformation;
	mat3 m_tbn;
	ShadingMode m_shading_mode;
};