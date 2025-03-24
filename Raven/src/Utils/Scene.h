#pragma once

#include "ravenpch.h"

#include "Core/Ray.h"
#include "Math/glm/glm.hpp"
#include "Math/glm/ext.hpp"
#include "Math/glm/gtx/norm.hpp"
#include "Core/Camera.h"
#include "Core/Material.h"
#include "Shapes/Mesh.h"
#include "Shapes/Triangle.h"
#include "Shapes/Sphere.h"
#include "Shapes/MeshInstance.h"
#include "Shapes/Vertex.h"

#include "Math/Transformations.h"

#include "Core/Texture.h"
#include "Core/ImageTexture.h"
#include "Core/CheckboardTexture.h"
#include "Core/PerlinTexture.h"
#include "Core/BRDF.h"

#include "Lights/SphereLight.h"

using glm::vec2;



namespace raven
{
	//class Mesh;
	//class Shape;
	//class Triangle;
	//class Sphere;
	//class Vertex;

	class Scene
	{
	// Functions
	public:
		Scene()
		{
			m_background_texture = nullptr;
			m_spherical_directional_light = nullptr;
			m_background_color = vec3(0.0f);
			m_ambient_light = vec3(0.0f);
			m_max_recursion = 1;
			m_shadow_ray_epsilon = 0.0001f;
			m_bvh = nullptr;

			m_cameras.reserve(1);
			m_vertex_data.reserve(16);
			m_tex_coords.reserve(16);

			m_vertices.reserve(16);

			m_sphere_lights.reserve(2);

			m_lights.reserve(8);

			m_materials.reserve(4);
			m_brdfs.reserve(4);
			m_textures.reserve(4);
			m_primitives.reserve(128);
			m_meshes.reserve(8);
			m_texture_paths.reserve(2);
			m_translations.reserve(2);
			m_scalings.reserve(2);
			m_rotations.reserve(2);
			m_composites.reserve(2);
		}

		~Scene();

	public:
		void parse_scene(const std::string& scene_name);

	// Vars
	public:
		vec3 m_background_color;
		ImageTexture* m_background_texture;
		SphericalDirectionalLight* m_spherical_directional_light;
		vec3 m_ambient_light;

		float m_shadow_ray_epsilon;
		int m_max_recursion;

		std::vector<Camera> m_cameras;
		std::vector<vec3> m_vertex_data;
		std::vector<vec2> m_tex_coords;

		std::vector<Vertex> m_vertices;

			
		std::vector<SphereLight> m_sphere_lights;

		// Test Lights

		std::vector<Light*> m_lights;


		std::vector<Material> m_materials;
		
		std::vector<BRDF*> m_brdfs;

		std::vector<Texture*> m_textures;
		std::vector<std::string> m_texture_paths;

		std::vector<Mesh> m_meshes;
		
		// BVH
		std::vector<Shape*> m_primitives;
		Shape* m_bvh;

		// Transformations
		std::vector<Transformations> m_translations;
		std::vector<Transformations> m_scalings;
		std::vector<Transformations> m_rotations;
		std::vector<Transformations> m_composites; // ?

	};
} 