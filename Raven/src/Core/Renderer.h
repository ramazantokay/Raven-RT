#pragma once

#include "ravenpch.h"
#include "Utils/Scene.h"
#include "Core/Camera.h"
#include "Core/Image.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Math/glm/glm.hpp"
#include "Math/glm/ext.hpp"
#include "Math/glm/gtx/norm.hpp"
#include "Math/glm/gtx/extended_min_max.hpp"
#include "Math/glm/ext/scalar_common.hpp"
#include "Math/Helper.h"

#define INTERSECTION_EPSILON 0.000001f
#define SHADOW_INTERSECTION_EPSILON 0.0001f
#define INTERSECTION_INFINITY std::numeric_limits<float>::infinity();

using glm::vec3;

class Renderer
{
public:
	Renderer(raven::Scene *scene);

	vec3 compute_color(const Ray& ray, raven::Camera cam, int p_x, int p_y, int recursion_depth);
	vec3 compute_color_for_path_tracer(const Ray& ray, raven::Camera cam, int p_x, int p_y, int recursion_depth);
	void render(int camera_index, std::vector<vec3>& pixels, const int row, const int height = 1);
	bool refract_ray(const vec3& dir, const vec3& normal, const float refract_index, vec3& transmit_dir) const; // Fundamentals of CG Book page 325.

public:
	raven::Scene* m_scene;
};