#pragma once

#include "Shape.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Math/glm/glm.hpp"
#include "Math/glm/ext.hpp"
#include "Math/glm/gtx/norm.hpp"
#include "Math/glm/gtc/matrix_access.hpp"

#include "Core/Ray.h"
#include "Core/RGB.h"
#include "Core/Texture.h"
#include "BBox.h"


#include "Math/Transformations.h"

using glm::vec3;
using glm::mat3;


class Sphere : public Shape
{
public:
	// Constructors
	Sphere() = default;
	Sphere(const vec3& _center, float _radius, const rgb& _color);
	Sphere(const vec3& _center, float _radius, const int& _mat);
	Sphere(const vec3& _center, float _radius, const int& _mat, raven::Texture* texture, raven::Texture* normal, raven::Transformations transformation);

	// Virtual Functions
	bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const override;
	bool shadow_hit(const Ray& ray, float t_min, float t_max, float time) const override;
	int get_material_id() const override;
	int get_texture_id() const override;
	const BBox& get_bbox() const override;

	// helpers

	vec2 get_uv(const vec3& p, raven::Texture* texture) const;
	mat3 calculate_tbn(vec3 p,  vec3 normal) const;


	// Variables
public:
	vec3 m_center;
	float m_radius;
	rgb m_color;
	int m_material_id;
	int m_texture_id;
	BBox m_bbox;
	raven::Transformations m_transformation;
	raven::Texture* m_texture; // Add this to Shape class
	raven::Texture* m_normal_texture; // Add this to Shape class
	raven::Texture* m_specular_texture; // Add this to Shape class
};