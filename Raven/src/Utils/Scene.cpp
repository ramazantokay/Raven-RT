#include "ravenpch.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "Scene.h"

#include "Utils/tinyxml2.h"
#include "Utils/happly.h"
#include <cmath>
#include <sstream>
#include <stdexcept>
#include <cassert>
#include <Lights/MeshLight.h>

//#include "Shapes/Triangle.h"
//#include "Shapes/Mesh.h"
//#include "Shapes/Sphere.h"
//#include "Shapes/MeshInstance.h"
//#include "Shapes/Vertex.h"


const float pi_over = RAVEN_PI / 180.0f;

// TODO : add cerr for each parsing step triangles, meshes, spheres etc.

void raven::Scene::parse_scene(const std::string& scene_name)
{
	// For optimization purposes, pre-allocate spaces


	tinyxml2::XMLDocument m_file;
	std::stringstream stream;

	auto res = m_file.LoadFile(scene_name.c_str());

	if (res)
	{
		std::cerr << "Couldn't open the scene file !!!!" << std::endl;
		throw std::runtime_error("Error: The xml file cannot be loaded.");
	}

	auto root = m_file.FirstChild();

	if (!root)
	{
		std::cerr << "Couldn't find the root !!!!" << std::endl;
		throw std::runtime_error("Error: Root is not found.");
	}

	// Get background color

	auto element = root->FirstChildElement("BackgroundColor");
	if (element)
	{
		stream << element->GetText() << std::endl;
		std::cerr << "BackgroundColor OK!" << std::endl;
	}
	else
	{
		stream << "0 0 0" << std::endl;
	}

	stream >> m_background_color.x >> m_background_color.y >> m_background_color.z;


	// Get shadow ray epsilon

	element = root->FirstChildElement("ShadowRayEpsilon");

	if (element)
	{
		stream << element->GetText() << std::endl;
		std::cerr << "ShadowRayEpsilon OK!" << std::endl;
	}
	else
	{
		stream << "0.001" << std::endl;
		std::cerr << "ShadowRayEpsilon OK!" << std::endl;
	}

	stream >> m_shadow_ray_epsilon;


	// Get maximum recursion depth

	element = root->FirstChildElement("MaxRecursionDepth");

	if (element)
	{
		std::cerr << "MaxRecursionDepth OK!" << std::endl;
		stream << element->GetText() << std::endl;
	}
	else
	{
		std::cerr << "MaxRecursionDepth OK!" << std::endl;
		stream << "0" << std::endl;
	}
	stream >> m_max_recursion;

	// Get Cameras

	element = root->FirstChildElement("Cameras");

	element = element->FirstChildElement("Camera");
	while (element)
	{
		auto child = element->FirstChildElement("Position");
		stream << child->GetText() << std::endl;

		child = element->FirstChildElement("Up");
		stream << child->GetText() << std::endl;

		child = element->FirstChildElement("NearDistance");
		stream << child->GetText() << std::endl;

		child = element->FirstChildElement("FocusDistance");

		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << 0 << std::endl;
		}

		child = element->FirstChildElement("ApertureSize");

		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << 0 << std::endl;
		}

		child = element->FirstChildElement("ImageResolution");
		stream << child->GetText() << std::endl;

		child = element->FirstChildElement("NumSamples");

		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << 1 << std::endl;
		}

		child = element->FirstChildElement("ImageName");
		stream << child->GetText() << std::endl;


		vec3 position, gaze, up;
		float l, r, t, b, near_dist, focus_dist, aperture_size;
		int w, h, num_samples;
		std::string name;

		stream >> position.x >> position.y >> position.z;
		stream >> up.x >> up.y >> up.z;
		stream >> near_dist;
		stream >> focus_dist;
		stream >> aperture_size;
		stream >> w >> h;
		stream >> num_samples;
		stream >> name;

		if (element->Attribute("type", "lookAt") != NULL)
		{
			child = element->FirstChildElement("Gaze");
			stream << child->GetText() << std::endl;

			child = element->FirstChildElement("FovY");
			stream << child->GetText() << std::endl;

			vec3 gaze_point;
			float fovy;
			stream >> gaze_point.x >> gaze_point.y >> gaze_point.z;
			stream >> fovy;

			float aspect_ratio = (1.0f * w / h); // multiply with 1.0f since it doesn't return floating point

			float radian_fovy = (fovy * (RAVEN_PI / 180.0f)) / 2;
			t = tanf(radian_fovy) * near_dist;
			b = -1.0f * t;

			r = t * aspect_ratio;
			l = -1.0f * r;

			gaze = normalize(gaze_point - position);

		}
		else // simple
		{

			child = element->FirstChildElement("Gaze");
			stream << child->GetText() << std::endl;

			child = element->FirstChildElement("NearPlane");
			stream << child->GetText() << std::endl;

			stream >> gaze.x >> gaze.y >> gaze.z;
			stream >> l >> r >> b >> t;
		}


		// ToneMap
		Tonemap tonemap;
		auto tonemap_child = element->FirstChildElement("Tonemap");
		if (tonemap_child)
		{
			if (child)
			{
				child = child->FirstChildElement("TMO");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "Photographic" << std::endl;
				}
			}

			child = tonemap_child->FirstChildElement("TMOOptions");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "0.18 1" << std::endl;
			}
			child = tonemap_child->FirstChildElement("Saturation");
			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "1.0" << std::endl;
			}
			child = tonemap_child->FirstChildElement("Gamma");

			if (child)
			{
				stream << child->GetText() << std::endl;
			}
			else
			{
				stream << "2.2" << std::endl;
			}
			std::string tmo;
			float saturation, gamma, key, burn;
			stream >> tmo;
			stream >> key >> burn >> saturation >> gamma;

			tonemap.m_type = TonemapType::Reinhard_Photographic;
			tonemap.m_burn = burn;
			tonemap.m_key_value = key;
			tonemap.m_saturation = saturation;
			tonemap.m_gamma = gamma;
		}
		else
		{
			tonemap.m_burn = 0.18;
			tonemap.m_key_value = 1.0;
			tonemap.m_saturation = 1.0;
			tonemap.m_gamma = 2.2;
			tonemap.m_type = TonemapType::None;
		}


		// Renderer Type

		child = element->FirstChildElement("Renderer");
		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "RayTracing" << std::endl;
		}

		// Renderer Params

		std::string renderer_type;
		stream >> renderer_type;
		RendererType r_type = (renderer_type == "RayTracing") ? RendererType::RayTracing : RendererType::PathTracing;

		bool nee = false;
		bool importance_sampling = false;
		bool russian_roulette = false;

		child = element->FirstChildElement("RendererParams");
		if (child)
		{
			if (r_type == RendererType::PathTracing)
			{
				std::string param;

				if (child->GetText())
				{
					stream << child->GetText() << std::endl;
					while (stream >> param)
					{
						if (param == "NextEventEstimation")
						{
							nee = true;
						}
						else if (param == "ImportanceSampling")
						{
							importance_sampling = true;
						}
						else if (param == "RussianRoulette")
						{
							russian_roulette = true;
						}
					}
				}
			}
		}

		child = element->FirstChildElement("SplittingFactor");
		if (child)
		{
			stream << child->GetText() << std::endl;
		}
		else
		{
			stream << "1" << std::endl;
		}

		int splitting_factor;
		stream >> splitting_factor;


		m_cameras.emplace_back(position, gaze, up, near_dist, focus_dist,
			aperture_size, l, r, t, b, name, w, h, num_samples,
			tonemap, r_type, nee, importance_sampling, russian_roulette, splitting_factor);
		element = element->NextSiblingElement("Camera");

		stream.clear();
	}

	if (!m_cameras.empty())
		std::cerr << "Camera OK!" << std::endl;


	//Get Lights


	// Point Light

	//PointLight point_light;
	tinyxml2::XMLElement* child;
	element = root->FirstChildElement("Lights");

	if (element)
	{
		child = element->FirstChildElement("AmbientLight");

		if (!child)
		{
			stream << "0 0 0" << std::endl;
		}
		else
		{
			stream << child->GetText() << std::endl;
		}

		stream >> m_ambient_light.x >> m_ambient_light.y >> m_ambient_light.z;


		element = element->FirstChildElement("PointLight");
		while (element)
		{
			child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Intensity");
			stream << child->GetText() << std::endl;

			vec3 position, intensity;
			stream >> position.x >> position.y >> position.z;
			stream >> intensity.x >> intensity.y >> intensity.z;

			m_lights.push_back(new PointLight(position, intensity));

			//m_point_lights.emplace_back(point_light);
			element = element->NextSiblingElement("PointLight");
		}





		/*if (!m_point_lights.empty())
			std::cerr << "PointLights OK!" << std::endl;*/

		stream.clear();


		// Area Light

		element = root->FirstChildElement("Lights");
		element = element->FirstChildElement("AreaLight");

		while (element)
		{
			child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Normal");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Size");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Radiance");
			stream << child->GetText() << std::endl;

			vec3 position(0.0f), normal(0.0f), radiance(0.0f);
			float _size;

			stream >> position.x >> position.y >> position.z;
			stream >> normal.x >> normal.y >> normal.z;
			stream >> _size;
			stream >> radiance.x >> radiance.y >> radiance.z;


			m_lights.push_back(new AreaLight(position, normal, _size, radiance));
			//m_area_lights.emplace_back(position, normal, _size, radiance);

			element = element->NextSiblingElement("AreaLight");
		}

		stream.clear();

		//if (!m_area_lights.empty())
		//	std::cerr << "AreaLights OK!" << std::endl;


		// Directional Light


		element = root->FirstChildElement("Lights");
		element = element->FirstChildElement("DirectionalLight");
		while (element)
		{
			child = element->FirstChildElement("Direction");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Radiance");
			stream << child->GetText() << std::endl;

			vec3 direction, radiance;
			stream >> direction.x >> direction.y >> direction.z;
			stream >> radiance.x >> radiance.y >> radiance.z;

			m_lights.push_back(new DirectionalLight(direction, radiance));

			//m_directional_lights.push_back(directional_light);
			element = element->NextSiblingElement("DirectionalLight");
		}

		/*if (!m_directional_lights.empty())
			std::cerr << "Directional Lights OK!" << std::endl;*/


			// Spot Light

		element = root->FirstChildElement("Lights");
		element = element->FirstChildElement("SpotLight");
		while (element)
		{
			child = element->FirstChildElement("Position");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Direction");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("Intensity");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("CoverageAngle");
			stream << child->GetText() << std::endl;
			child = element->FirstChildElement("FalloffAngle");
			stream << child->GetText() << std::endl;

			vec3 position, direction, intensity;;
			float coverage_angle, falloff_angle;

			stream >> position.x >> position.y >> position.z;
			stream >> direction.x >> direction.y >> direction.z;
			stream >> intensity.x >> intensity.y >> intensity.z;
			stream >> coverage_angle >> falloff_angle;

			// convert to radians
			coverage_angle = (coverage_angle * RAVEN_PI) / 180.0f;
			falloff_angle = (falloff_angle * RAVEN_PI) / 180.0f;

			m_lights.push_back(new SpotLight(position, direction, intensity, coverage_angle, falloff_angle));
			//m_spot_lights.emplace_back(position, direction, intensity, coverage_angle, falloff_angle);

			element = element->NextSiblingElement("SpotLight");
		}



		// Get Spherical Directional Light

		element = root->FirstChildElement("Lights");


		element = element->FirstChildElement("SphericalDirectionalLight");

		while (element)
		{
			SphericalDirectionalLightType type = SphericalDirectionalLightType::None;

			if (element->Attribute("type", "latlong") != NULL)
			{
				type = SphericalDirectionalLightType::LatLong;
			}
			else if (element->Attribute("type", "probe") != NULL)
			{
				type = SphericalDirectionalLightType::Probe;
			}
			else
			{
				type = SphericalDirectionalLightType::None;
			}


			int imageId;
			child = element->FirstChildElement("ImageId");
			stream << child->GetText() << std::endl;
			stream >> imageId;

			/*SphericalDirectionalLight* light = new SphericalDirectionalLight(m_textures[imageId], );
			*/

			// cast ImageTexture

			m_lights.push_back(new SphericalDirectionalLight(m_texture_paths[imageId - 1], type));

			m_spherical_directional_light = new SphericalDirectionalLight(m_texture_paths[imageId - 1], type);

			element = element->NextSiblingElement("SphericalDirectionalLight");
		}

	}
	stream.clear();

	// Get BRDFs
	// TODO: change normalized part 
	element = root->FirstChildElement("BRDFs");
	if (element)
	{
		child = element->FirstChildElement("OriginalPhong");
		while (child)
		{
			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			m_brdfs.push_back(new PhongBRDF(exponent));
			child = child->NextSiblingElement("OriginalPhong");
		}

		child = element->FirstChildElement("ModifiedPhong");
		while (child)
		{
			//bool normalized;
			bool normalized = child->BoolAttribute("normalized", false);


			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			m_brdfs.push_back(new ModifiedPhongBRDF(exponent, normalized));
			child = child->NextSiblingElement("ModifiedPhong");
		}

		child = element->FirstChildElement("OriginalBlinnPhong");
		while (child)
		{
			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			m_brdfs.push_back(new BlinnPhongBRDF(exponent));
			child = child->NextSiblingElement("OriginalBlinnPhong");
		}


		child = element->FirstChildElement("ModifiedBlinnPhong");
		while (child)
		{
			bool normalized = child->BoolAttribute("normalized", false);

			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			m_brdfs.push_back(new ModifiedBlinnPhongBRDF(exponent, normalized));
			child = child->NextSiblingElement("ModifiedBlinnPhong");
		}

		child = element->FirstChildElement("TorranceSparrow");
		while (child)
		{
			bool kdfresnel = child->BoolAttribute("kdfresnel", false);

			auto exponentElement = child->FirstChildElement("Exponent");
			stream << exponentElement->GetText() << std::endl;
			float exponent;
			stream >> exponent;

			m_brdfs.push_back(new TorranceSparrowBRDF(exponent, kdfresnel));
			child = child->NextSiblingElement("TorranceSparrow");
		}
	}

	stream.clear();


	//Get Materials
	Material material;
	element = root->FirstChildElement("Materials");

	element = element->FirstChildElement("Material");

	while (element)
	{

		if (element->Attribute("type", "mirror") != NULL)
		{
			material.m_type = MaterialType::Mirror;
		}
		else if (element->Attribute("type", "dielectric") != NULL)
		{
			material.m_type = MaterialType::Dielectric;
		}
		else if (element->Attribute("type", "conductor") != NULL)
		{
			material.m_type = MaterialType::Conductor;
		}
		else
		{
			material.m_type = MaterialType::Simple;
		}

		if (element->Attribute("degamma", "true") != NULL)
		{
			material.m_degamma = true;
		}

		if (element->Attribute("degamma", "false") != NULL)
		{
			material.m_degamma = false;
		}

		const char* brdf_id = element->Attribute("BRDF");
		if (brdf_id)
		{
			material.m_brdf_id = std::stoi(std::string(brdf_id));
		}
		else
		{
			material.m_brdf_id = 0;
		}

		child = element->FirstChildElement("AmbientReflectance");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("DiffuseReflectance");
		stream << child->GetText() << std::endl;
		child = element->FirstChildElement("SpecularReflectance");
		stream << child->GetText() << std::endl;

		child = element->FirstChildElement("AbsorptionCoefficient");
		if (child)
			stream << child->GetText() << std::endl;
		else
			stream << "0 0 0 " << std::endl;

		child = element->FirstChildElement("MirrorReflectance");
		if (child)
			stream << child->GetText() << std::endl;
		else
			stream << "0 0 0" << std::endl;

		child = element->FirstChildElement("PhongExponent");
		if (child)
			stream << child->GetText() << std::endl;
		else
			stream << "0" << std::endl;


		child = element->FirstChildElement("RefractionIndex");
		if (child)
			stream << child->GetText() << std::endl;
		else
			stream << "0" << std::endl;

		child = element->FirstChildElement("AbsorptionIndex");
		if (child)
			stream << child->GetText() << std::endl;
		else
			stream << "0" << std::endl;

		child = element->FirstChildElement("Roughness");

		if (child)
			stream << child->GetText() << std::endl;
		else
			stream << "0" << std::endl;

		// TODO: no need to create material object, use vars and put them in emplace back function. create new constuctor
		stream >> material.m_ambient_reflectance.x >> material.m_ambient_reflectance.y >> material.m_ambient_reflectance.z;
		stream >> material.m_diffuse_reflectance.x >> material.m_diffuse_reflectance.y >> material.m_diffuse_reflectance.z;
		stream >> material.m_specular_reflectance.x >> material.m_specular_reflectance.y >> material.m_specular_reflectance.z;
		stream >> material.m_absorption_coefficient.x >> material.m_absorption_coefficient.y >> material.m_absorption_coefficient.z;
		stream >> material.m_mirror_reflectance.x >> material.m_mirror_reflectance.y >> material.m_mirror_reflectance.z;
		stream >> material.m_phong_exp;
		stream >> material.m_refraction_index;
		stream >> material.m_absorption_index;
		stream >> material.m_roughness;

		m_materials.emplace_back(material);
		element = element->NextSiblingElement("Material");
		stream.clear();

	}

	if (!m_materials.empty())
		std::cerr << "Materials OK!" << std::endl;

	// Textures

	element = root->FirstChildElement("Textures");
	if (element)
	{
		// Get Images
		std::string textures_dir = "src/Scenes/hw6/sponza/inputs/";

		child = element->FirstChildElement("Images");
		if (child)
		{
			child = child->FirstChildElement("Image");
			while (child)
			{
				std::string texture_name;
				stream << child->GetText() << std::endl;
				stream >> texture_name;
				std::string texture_path = textures_dir + texture_name;
				m_texture_paths.push_back(texture_path);
				child = child->NextSiblingElement("Image");
			}
		}

		// Texture Maps
		element = element->FirstChildElement("TextureMap");
		while (element)
		{
			if (element->Attribute("type", "image") != NULL)
			{
				// Image Textures

				int image_id;
				child = element->FirstChildElement("ImageId");
				stream << child->GetText() << std::endl;
				stream >> image_id;

				child = element->FirstChildElement("DecalMode");
				stream << child->GetText() << std::endl;

				child = element->FirstChildElement("Interpolation");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "bilinear" << std::endl;
				}

				child = element->FirstChildElement("Normalizer");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "255.0" << std::endl;
				}

				child = element->FirstChildElement("BumpFactor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				std::string interpolation_type, decalmode;
				float normalizer, bumpfactor;
				stream >> decalmode;
				stream >> interpolation_type;
				stream >> normalizer;
				stream >> bumpfactor;

				ImageTexture* image_texture = new ImageTexture(m_texture_paths[image_id - 1], interpolation_type, decalmode, normalizer, bumpfactor);
				m_textures.push_back(image_texture);

				// Background texture
				if (image_texture->get_decal_value() == DecalMode::ReplaceBackground)
				{
					m_background_texture = image_texture;
				}
			}
			else if (element->Attribute("type", "checkerboard") != NULL)
			{
				// Checkboard Texture


				child = element->FirstChildElement("DecalMode");
				stream << child->GetText() << std::endl;

				child = element->FirstChildElement("BlackColor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "0 0 0" << std::endl;
				}

				child = element->FirstChildElement("WhiteColor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "255 255 255" << std::endl;
				}

				child = element->FirstChildElement("Scale");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				child = element->FirstChildElement("Offset");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "0.01" << std::endl;
				}

				std::string  decalmode;
				vec3 white, black;
				float scale, offset;
				stream >> decalmode;
				stream >> black.x >> black.y >> black.z;
				stream >> white.x >> white.y >> white.z;
				stream >> scale >> offset;

				CheckboardTexture* checkboard = new CheckboardTexture(black, white, scale, offset, decalmode);
				m_textures.push_back(checkboard);

			}
			else if (element->Attribute("type", "perlin") != NULL)
			{
				// Perlin Texture

				child = element->FirstChildElement("DecalMode");
				stream << child->GetText() << std::endl;
				child = element->FirstChildElement("NoiseConversion");
				stream << child->GetText() << std::endl;

				child = element->FirstChildElement("NoiseScale");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				child = element->FirstChildElement("BumpFactor");
				if (child)
				{
					stream << child->GetText() << std::endl;
				}
				else
				{
					stream << "1.0" << std::endl;
				}

				std::string decal_mode, noise_conversion;
				float noise_scale, bump_factor;
				stream >> decal_mode;
				stream >> noise_conversion;
				stream >> noise_scale;
				stream >> bump_factor;

				PerlinTexture* perlin = new PerlinTexture(decal_mode, noise_conversion, noise_scale, bump_factor);
				m_textures.push_back(perlin);
			}


			element = element->NextSiblingElement("TextureMap");
		}
	}
	stream.clear();




	// Transformations yowww

	element = root->FirstChildElement("Transformations");

	if (element)
	{
		// Get Translation

		child = element->FirstChildElement("Translation");
		while (child)
		{
			const char* id;
			id = child->Attribute("id");

			vec3 trans_vec(0.0f);
			stream << child->GetText() << std::endl;
			stream >> trans_vec.x >> trans_vec.y >> trans_vec.z;

			m_translations.emplace_back(std::stoi(std::string(id)), TransformationType::Translation, trans_vec);
			child = child->NextSiblingElement("Translation");
		}

		// Get Scaling 

		child = element->FirstChildElement("Scaling");
		while (child)
		{
			const char* id;
			id = child->Attribute("id");

			vec3 scaling_vec(0.0f);
			stream << child->GetText() << std::endl;
			stream >> scaling_vec.x >> scaling_vec.y >> scaling_vec.z;

			m_scalings.emplace_back(std::stoi(std::string(id)), TransformationType::Scaling, scaling_vec);
			child = child->NextSiblingElement("Scaling");
		}

		// Get Rotation

		child = element->FirstChildElement("Rotation");
		while (child)
		{
			const char* id;
			id = child->Attribute("id");

			vec3 rotation_vec(0.0f);
			float _angle;
			stream << child->GetText() << std::endl;
			stream >> _angle >> rotation_vec.x >> rotation_vec.y >> rotation_vec.z;

			m_rotations.emplace_back(std::stoi(std::string(id)), TransformationType::Rotation, _angle, rotation_vec);
			child = child->NextSiblingElement("Rotation");
		}
	}

	//Get Vertex Data
	vec3 vertex(0.0f);

	element = root->FirstChildElement("VertexData");
	if (element)
	{
		const char* bin_file = element->Attribute("binaryFile");
		if (bin_file)
		{
			std::string bin_file_path = "src/Scenes/hw6/sponza/inputs/" + std::string(bin_file);
			// Parse bin file
			FILE* fd = fopen(bin_file_path.c_str(), "rb");
			int N;
			fread(&N, sizeof(int), 1, fd);
			for	(int i = 0; i < N; i++)
			{
				fread(&vertex, sizeof(float), 3, fd);
				m_vertices.emplace_back(vertex);
			}

		}
		else
		{ 
		stream << element->GetText() << std::endl;

		while (!(stream >> vertex.x).eof())
		{
			stream >> vertex.y >> vertex.z;

			// New Vertex Class - Experimental 
			m_vertices.emplace_back(vertex);


			m_vertex_data.emplace_back(vertex);
		}
		}

	}
	stream.clear();

	if (!m_vertices.empty())
	{
		std::cerr << "Vertex Data OK!" << std::endl;
	}

	// Texture Coords
	vec2 _uv(0.0f);
	element = root->FirstChildElement("TexCoordData");
	if (element)
	{
		const char* bin_file = element->Attribute("binaryFile");
		if (bin_file)
		{
			std::string bin_file_path = "src/Scenes/hw6/sponza/inputs/" + std::string(bin_file);
			// Parse bin file
			FILE* fd = fopen(bin_file_path.c_str(), "rb");
			int N;
			fread(&N, sizeof(int), 1, fd);
			for (int i = 0; i < N; i++)
			{
				fread(&_uv, sizeof(float), 2, fd);
				m_vertices[i].m_uv = _uv;

				m_tex_coords.push_back(_uv);
			}

		}
		else
		{

			stream << element->GetText() << std::endl;
			vec2 tex_coord(0.0f);
			int uv_counter = 0;
			while (!(stream >> tex_coord.x).eof())
			{
				stream >> tex_coord.y;

				m_vertices[uv_counter++].m_uv = tex_coord;


				m_tex_coords.push_back(tex_coord);

			}
		}
	}
	else
	{
		std::cerr << "TexCoord Data is not found!" << std::endl;
	}

	stream.clear();

	// Get Meshes

	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Mesh");

	while (element)
	{
		// Shading Mode
		ShadingMode s_mode = ShadingMode::Flat;

		if (element->Attribute("shadingMode", "smooth") != NULL)
		{
			s_mode = ShadingMode::Smooth;
		}
		else if (element->Attribute("shadingMode", "flat") != NULL)
		{
			s_mode = ShadingMode::Flat;
		}
		else
		{
			s_mode = ShadingMode::Flat;
		}

		// Material 

		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;


		int material_id;
		stream >> material_id;


		int texture_id = -1;
		child = element->FirstChildElement("Textures");

		raven::Texture* diffuse_texture = NULL;
		raven::Texture* normal_texture = NULL;
		raven::Texture* specular_texture = NULL; // TODO: will be implemented

		if (child)
		{
			int diffuse_texture_id = -1;
			int normal_texture_id = -1;

			stream << child->GetText() << std::endl;
			stream >> diffuse_texture_id >> normal_texture_id;

			if (diffuse_texture_id > 0 && normal_texture_id > 0)
			{
				diffuse_texture = m_textures[diffuse_texture_id - 1]; 
				//normal_texture = m_textures[normal_texture_id - 1]; // TODO: don't forget to uncomment when you add normal texture
			}

			if (diffuse_texture_id > 0 && normal_texture_id == -1)
			{
				if ((m_textures[diffuse_texture_id - 1]->m_is_normal_map) == false)
				{
					diffuse_texture = m_textures[diffuse_texture_id - 1];
				}
				else
				{
					//normal_texture = m_textures[diffuse_texture_id - 1]; // TODO: don't forget to uncomment when you add normal texture
				}
			}
		}

		stream.clear();

		// Transformation

		child = element->FirstChildElement("Transformations");

		glm::mat4 M(1.0f);

		if (child) // calculate composite matrix
		{
			char _type;
			int _i;
			stream.clear();
			stream << child->GetText() << std::endl;

			while (!(stream >> _type).eof())
			{
				stream >> _i;
				_i--;

				if (_type == 't')
				{
					M = m_translations[_i].m_transformations * M;
				}
				if (_type == 's')
				{
					M = m_scalings[_i].m_transformations * M;
				}
				if (_type == 'r')
				{
					M = m_rotations[_i].m_transformations * M;
				}
			}
			stream.clear();
		}

		stream.clear();


		// Faces

		child = element->FirstChildElement("Faces");
		int vertexOffset = child->IntAttribute("vertexOffset", 0);

		const char* ply_file_name = child->Attribute("plyFile");
		const char* bin_file = child->Attribute("binaryFile");


		char dir[100] = "src/Scenes/hw6/sponza/inputs/"; // TODO: get from args
		std::vector<Shape*> triangles;

		if (ply_file_name)
		{
			// Parse ply file TODO: write ply parser helper function
			// src/Scene/

			strcat(dir, ply_file_name);

			happly::PLYData ply_file(dir);

			std::vector<std::string> names = ply_file.getElementNames(); // for debug purpose, list all the names 

			std::vector<std::array<double, 3>> vertices = ply_file.getVertexPositions();
			std::vector<std::vector<size_t>> face = ply_file.getFaceIndices<size_t>();


			//TODO: add tex coords

			bool is_uv_exist = ply_file.getElement("vertex").hasProperty("u");
			std::vector<double> u_pos;
			std::vector<double> v_pos;


			// Get Vertices and UVs put them on m_vertices

			// Get Vertices
			int num_existing_vertices = m_vertices.size();

			for (int i = 0; i < vertices.size(); i++)
			{
				m_vertices.emplace_back(vec3(vertices[i][0], vertices[i][1], vertices[i][2]));
			}


			if (is_uv_exist)
			{
				u_pos = ply_file.getElement("vertex").getProperty<double>("u");
				v_pos = ply_file.getElement("vertex").getProperty<double>("v");

				std::vector<std::array<double, 2>> res(u_pos.size());
				for (size_t i = 0; i < res.size(); i++)
				{
					res[i][0] = u_pos[i];
					res[i][1] = v_pos[i];
				}

				std::vector<std::array<double, 2>> tex_coords = res;

				for (int i = 0; i < tex_coords.size(); i++)
				{
					vec2 uv = vec2(tex_coords[i][0], tex_coords[i][1]);
					m_vertices[i + num_existing_vertices].m_uv = uv;
					m_tex_coords.push_back(uv);
				}

			}
			for (size_t i = 0; i < face.size(); i++)
			{
				if (face[i].size() == 3) // For Triangle Faces
				{

					auto v1 = vertices[face[i][0]];
					auto v2 = vertices[face[i][1]];
					auto  v3 = vertices[face[i][2]];

					vec3 temp_v1(v1[0], v1[1], v1[2]);
					vec3 temp_v2(v2[0], v2[1], v2[2]);
					vec3 temp_v3(v3[0], v3[1], v3[2]);

					triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][1], num_existing_vertices + face[i][2], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));

					//if (is_uv_exist)
					//{
					//	/*triangles.emplace_back(new Triangle(temp_v1, temp_v2, temp_v3,
					//		m_tex_coords[face[i][0]], m_tex_coords[face[i][1]], m_tex_coords[face[i][2]], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));*/

					//}
					//else
					//{
					//	triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][1], num_existing_vertices + face[i][2], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));

					//triangles.emplace_back(new Triangle(temp_v1, temp_v2, temp_v3, 
					//	vec2(0.0f), vec2(0.0f), vec2(0.0f), s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));
					//}


				}

				if (face[i].size() == 4) // For Quad Faces
				{
					auto v1 = vertices[face[i][0]];
					auto v2 = vertices[face[i][1]];
					auto  v3 = vertices[face[i][2]];
					auto  v4 = vertices[face[i][3]];

					vec3 temp_v1(v1[0], v1[1], v1[2]);
					vec3 temp_v2(v2[0], v2[1], v2[2]);
					vec3 temp_v3(v3[0], v3[1], v3[2]);
					vec3 temp_v4(v4[0], v4[1], v4[2]);

					triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][1], num_existing_vertices + face[i][2], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));
					triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][2], num_existing_vertices + face[i][3], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));


					//triangles.emplace_back(new Triangle(temp_v1, temp_v2, temp_v3, material_id, texture_id));
					//triangles.emplace_back(new Triangle(temp_v1, temp_v3, temp_v4, material_id, texture_id));
				}
			}
		}
		else if (bin_file)
		{
			std::string bin_file_path = "src/Scenes/hw6/sponza/inputs/" + std::string(bin_file);
			// Parse bin file
			FILE* fd = fopen(bin_file_path.c_str(), "rb");
			int N;
			fread(&N, sizeof(int), 1, fd);
			for (int i = 0; i < N; i++)
			{
				int v0_id, v1_id, v2_id;
				fread(&v0_id, sizeof(int), 1, fd);
				fread(&v1_id, sizeof(int), 1, fd);
				fread(&v2_id, sizeof(int), 1, fd);

				triangles.emplace_back(new Triangle(v0_id  + vertexOffset, v1_id + vertexOffset, v2_id  + vertexOffset, s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));
			}
		}
		else

		{
			stream << child->GetText() << std::endl;

			int v0_id, v1_id, v2_id;

			while (!(stream >> v0_id).eof())
			{
				stream >> v1_id >> v2_id;
				//std::cerr << "Mesh Faces" << " " << v0_id << " " << v1_id << " " << v2_id << std::endl;
				//triangles.emplace_back(new Triangle(m_vertex_data[v0_id - 1], m_vertex_data[v1_id - 1], m_vertex_data[v2_id - 1],
				//	m_tex_coords[v0_id - 1], m_tex_coords[v1_id - 1], m_tex_coords[v2_id - 1], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));
				Triangle* new_triangle = new Triangle(v0_id - 1 + vertexOffset, v1_id - 1 + vertexOffset, v2_id - 1 + vertexOffset, s_mode, material_id, diffuse_texture, normal_texture, Transformations(M));

				if (s_mode == ShadingMode::Smooth)
				{
					m_vertices[v0_id - 1 + vertexOffset].add_vertex_normal(new_triangle->m_surface_normal);
					m_vertices[v1_id - 1 + vertexOffset].add_vertex_normal(new_triangle->m_surface_normal);
					m_vertices[v2_id - 1 + vertexOffset].add_vertex_normal(new_triangle->m_surface_normal);
				}

				//if (v0_id == 19 || v1_id == 19 || v2_id == 19)
				//if (length(new_triangle->m_surface_normal) <= 0.0f)
				// Degenerate Triangle cases
				if (glm::isnan(m_vertices[v0_id - 1 + vertexOffset].m_normal) == glm::bvec3(true) || glm::isnan(m_vertices[v1_id - 1 + vertexOffset].m_normal) == glm::bvec3(true) ||
					glm::isnan(m_vertices[v2_id - 1 + vertexOffset].m_normal) == glm::bvec3(true))
				{
					std::cerr << "Face 19 " << std::endl;
					std::cerr << "Normal " << new_triangle->m_surface_normal.x << " " << new_triangle->m_surface_normal.y << " " << new_triangle->m_surface_normal.z << std::endl;
					std::cerr << "Vertex v0 " << m_vertices[v0_id - 1 + vertexOffset].m_position.x << " " << m_vertices[v0_id - 1 + vertexOffset].m_position.y << " " << m_vertices[v0_id + vertexOffset - 1].m_position.z << std::endl;
					std::cerr << "Vertex v1 " << m_vertices[v1_id - 1 + vertexOffset].m_position.x << " " << m_vertices[v1_id - 1 + vertexOffset].m_position.y << " " << m_vertices[v1_id + vertexOffset - 1].m_position.z << std::endl;
					std::cerr << "Vertex v2 " << m_vertices[v2_id - 1 + vertexOffset].m_position.x << " " << m_vertices[v2_id - 1 + vertexOffset].m_position.y << " " << m_vertices[v2_id + vertexOffset - 1].m_position.z << std::endl;

					std::cerr << "Length v0 - v1 " << length(m_vertices[v0_id - 1 + vertexOffset].m_position - m_vertices[v1_id - 1 + vertexOffset].m_position) << std::endl;
					std::cerr << "Length v1 - v2 " << length(m_vertices[v1_id - 1 + vertexOffset].m_position - m_vertices[v2_id - 1 + vertexOffset].m_position) << std::endl;

				}


				triangles.emplace_back(new_triangle);
			}

			stream.clear();
		}

		m_meshes.emplace_back(material_id, diffuse_texture, normal_texture, triangles, Transformations(M));

		//m_primitives.emplace_back(new Mesh(material_id, triangles, Transformations(M)));

		element = element->NextSiblingElement("Mesh");


	}
	// dDebug purpose
	for (Mesh& m : m_meshes)
	{
		//m_primitives
		m_primitives.emplace_back(new MeshInstance(m.m_material_id, m.m_texture, m.m_normal_texture, &m, m.m_transformation, false)); // TODO: add impilict value
	}



	stream.clear();

	// Get MeshInstance


	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("MeshInstance");

	while (element)
	{
		Mesh& base_mesh = m_meshes[element->IntAttribute("baseMeshId") - 1];
		child = element->FirstChildElement("Material");

		int material_id;
		stream << child->GetText() << std::endl;
		stream >> material_id;

		int texture_id = -1;
		child = element->FirstChildElement("Textures");

		raven::Texture* diffuse_texture = nullptr;
		raven::Texture* normal_texture = nullptr;
		raven::Texture* specular_texture = nullptr;

		if (child)
		{
			int diffuse_texture_id = -1;
			int normal_texture_id = -1;

			stream << child->GetText() << std::endl;
			stream >> diffuse_texture_id >> normal_texture_id;

			if (diffuse_texture_id > 0 && normal_texture_id > 0)
			{
				diffuse_texture = m_textures[diffuse_texture_id - 1];
				normal_texture = m_textures[normal_texture_id - 1];
			}

			if (diffuse_texture_id > 0 && normal_texture_id == -1)
			{
				if ((m_textures[diffuse_texture_id - 1]->m_is_normal_map) == false)
				{
					diffuse_texture = m_textures[diffuse_texture_id - 1];
				}
				else
				{
					normal_texture = m_textures[diffuse_texture_id - 1];
				}
			}
		}

		stream.clear();

		const char* reset_transform = element->Attribute("resetTransform");

		if (reset_transform == NULL)
		{
			reset_transform = "false";
		}

		glm::mat4 M_base;

		if (reset_transform && std::string(reset_transform) == std::string("false"))
		{
			M_base = base_mesh.m_transformation.m_transformations;
			//M_base = glm::mat4(1.0f);

		}
		else
		{
			M_base = glm::mat4(1.0f);

			//glm::mat4 M_base = base_mesh.m_transformation.m_transformations;
			// TODO: I need to reset base mesh transformation, I guess
		}

		// Transformation

		child = element->FirstChildElement("Transformations");


		if (child) // calculate composite matrix
		{
			char _type;
			int _i;
			stream.clear();
			stream << child->GetText() << std::endl;

			while (!(stream >> _type).eof())
			{
				stream >> _i;
				_i--;

				if (_type == 't')
				{
					M_base = m_translations[_i].m_transformations * M_base;
					//M_base = M_base * m_translations[_i].m_transformations ;
				}
				if (_type == 's')
				{
					M_base = m_scalings[_i].m_transformations * M_base;
					//M_base = M_base * m_scalings[_i].m_transformations ;
				}
				if (_type == 'r')
				{
					M_base = m_rotations[_i].m_transformations * M_base;
					//M_base = M_base* m_rotations[_i].m_transformations ;
				}
			}
			stream.clear();
		}

		stream.clear();

		vec3 motion_vector;

		child = element->FirstChildElement("MotionBlur");
		if (child)
		{
			stream << child->GetText() << std::endl;
			stream >> motion_vector.x >> motion_vector.y >> motion_vector.z;
		}

		m_primitives.emplace_back(new MeshInstance(material_id, diffuse_texture, normal_texture, &base_mesh, Transformations(M_base), (std::string(reset_transform) == std::string("true") ? true : false), motion_vector));

		element = element->NextSiblingElement("MeshInstance");

	}

	stream.clear();


	//Get Triangles

	element = root->FirstChildElement("Objects");

	element = element->FirstChildElement("Triangle");
	while (element)
	{
		// Shading Mode
		ShadingMode s_mode = ShadingMode::Flat;

		if (element->Attribute("shadingMode", "smooth") != NULL)
		{
			s_mode = ShadingMode::Smooth;
		}
		else if (element->Attribute("shadingMode", "flat") != NULL)
		{
			s_mode = ShadingMode::Flat;
		}
		else
		{
			s_mode = ShadingMode::Flat;
		}


		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		int mat_id;
		stream >> mat_id;

		int texture_id = -1;
		child = element->FirstChildElement("Textures");

		raven::Texture* diffuse_texture = NULL;
		raven::Texture* normal_texture = NULL;
		raven::Texture* specular_texture = NULL;

		if (child)
		{
			int diffuse_texture_id = -1;
			int normal_texture_id = -1;

			stream << child->GetText() << std::endl;
			stream >> diffuse_texture_id >> normal_texture_id;


			if (diffuse_texture_id > 0 && normal_texture_id > 0)
			{
				diffuse_texture = m_textures[diffuse_texture_id - 1];
				normal_texture = m_textures[normal_texture_id - 1];
			}

			if (diffuse_texture_id > 0 && normal_texture_id == -1)
			{
				if ((m_textures[diffuse_texture_id - 1]->m_is_normal_map) == false)
				{
					diffuse_texture = m_textures[diffuse_texture_id - 1];
				}
				else
				{
					normal_texture = m_textures[diffuse_texture_id - 1];
				}
			}
		}

		stream.clear();

		// Transformation

		child = element->FirstChildElement("Transformations");

		glm::mat4 M(1.0f);

		if (child) // calculate composite matrix
		{
			char _type;
			int _i;
			stream.clear();
			stream << child->GetText() << std::endl;

			while (!(stream >> _type).eof())
			{
				stream >> _i;
				_i--;

				if (_type == 't')
				{
					M = m_translations[_i].m_transformations * M;
				}
				if (_type == 's')
				{
					M = m_scalings[_i].m_transformations * M;
				}
				if (_type == 'r')
				{
					M = m_rotations[_i].m_transformations * M;
				}
			}
			stream.clear();
		}

		stream.clear();

		child = element->FirstChildElement("Indices");
		stream << child->GetText() << std::endl;

		int v0, v1, v2;

		stream >> v0 >> v1 >> v2;

		m_primitives.emplace_back(new Triangle(m_vertex_data[v0 - 1], m_vertex_data[v1 - 1], m_vertex_data[v2 - 1],
			m_tex_coords[v0 - 1], m_tex_coords[v1 - 1], m_tex_coords[v2 - 1], s_mode, mat_id, diffuse_texture, normal_texture, Transformations(M)));

		element = element->NextSiblingElement("Triangle");
	}


	// Get Spheres
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("Sphere");
	while (element)
	{
		int material_id, center;
		float radius;

		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> material_id;
		//material_id--;

		int texture_id = -1;
		child = element->FirstChildElement("Textures");

		raven::Texture* diffuse_texture = nullptr;
		raven::Texture* normal_texture = nullptr;
		raven::Texture* specular_texture = nullptr; // TODO: will be implemented

		if (child)
		{
			int diffuse_texture_id = -1;
			int normal_texture_id = -1;

			stream << child->GetText() << std::endl;
			stream >> diffuse_texture_id >> normal_texture_id;

			if (diffuse_texture_id > 0 && normal_texture_id > 0)
			{
				diffuse_texture = m_textures[diffuse_texture_id - 1];
				normal_texture = m_textures[normal_texture_id - 1];
			}

			if (diffuse_texture_id > 0 && normal_texture_id == -1)
			{
				if ((m_textures[diffuse_texture_id - 1]->m_is_normal_map) == false)
				{
					diffuse_texture = m_textures[diffuse_texture_id - 1];
				}
				else
				{
					normal_texture = m_textures[diffuse_texture_id - 1];
				}
			}
		}

		stream.clear();

		// Transformation

		child = element->FirstChildElement("Transformations");

		glm::mat4 M(1.0f);

		if (child) // calculate composite matrix
		{
			char _type;
			int _i;
			stream.clear();
			stream << child->GetText() << std::endl;

			while (!(stream >> _type).eof())
			{
				stream >> _i;
				_i--;

				if (_type == 't')
				{
					M = m_translations[_i].m_transformations * M;
				}
				if (_type == 's')
				{
					M = m_scalings[_i].m_transformations * M;
				}
				if (_type == 'r')
				{
					M = m_rotations[_i].m_transformations * M;
				}
			}
			stream.clear();
		}

		stream.clear();

		child = element->FirstChildElement("Center");
		stream << child->GetText() << std::endl;
		stream >> center;
		const vec3& center_sphere = m_vertex_data[center - 1];

		child = element->FirstChildElement("Radius");
		stream << child->GetText() << std::endl;
		stream >> radius;

		m_primitives.emplace_back(new Sphere(center_sphere, radius, material_id, diffuse_texture, normal_texture, Transformations(M)));

		element = element->NextSiblingElement("Sphere");
	}


	// Get Sphere Light
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("LightSphere");
	while (element)
	{
		int material_id, center;
		float radius;


		child = element->FirstChildElement("Radiance");
		stream << child->GetText() << std::endl;
		vec3 radiance;
		stream >> radiance.x >> radiance.y >> radiance.z;

		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> material_id;
		//material_id--;

		int texture_id = -1;
		child = element->FirstChildElement("Textures");

		raven::Texture* diffuse_texture = nullptr;
		raven::Texture* normal_texture = nullptr;
		raven::Texture* specular_texture = nullptr; // TODO: will be implemented

		if (child)
		{
			int diffuse_texture_id = -1;
			int normal_texture_id = -1;

			stream << child->GetText() << std::endl;
			stream >> diffuse_texture_id >> normal_texture_id;

			if (diffuse_texture_id > 0 && normal_texture_id > 0)
			{
				diffuse_texture = m_textures[diffuse_texture_id - 1];
				normal_texture = m_textures[normal_texture_id - 1];
			}

			if (diffuse_texture_id > 0 && normal_texture_id == -1)
			{
				if ((m_textures[diffuse_texture_id - 1]->m_is_normal_map) == false)
				{
					diffuse_texture = m_textures[diffuse_texture_id - 1];
				}
				else
				{
					normal_texture = m_textures[diffuse_texture_id - 1];
				}
			}
		}

		stream.clear();

		// Transformation

		child = element->FirstChildElement("Transformations");

		glm::mat4 M(1.0f);

		if (child) // calculate composite matrix
		{
			char _type;
			int _i;
			stream.clear();
			stream << child->GetText() << std::endl;

			while (!(stream >> _type).eof())
			{
				stream >> _i;
				_i--;

				if (_type == 't')
				{
					M = m_translations[_i].m_transformations * M;
				}
				if (_type == 's')
				{
					M = m_scalings[_i].m_transformations * M;
				}
				if (_type == 'r')
				{
					M = m_rotations[_i].m_transformations * M;
				}
			}
			stream.clear();
		}

		stream.clear();

		child = element->FirstChildElement("Center");
		stream << child->GetText() << std::endl;
		stream >> center;
		const vec3& center_sphere = m_vertex_data[center - 1];
		
		child = element->FirstChildElement("Radius");
		stream << child->GetText() << std::endl;
		stream >> radius;

		SphereLight* sphere_light = new SphereLight(center_sphere, radius, material_id, diffuse_texture, normal_texture, Transformations(M), radiance);

		m_primitives.emplace_back(sphere_light);
		m_lights.push_back(sphere_light);

		//m_sphere_lights.push_back(sphere_light);

		element = element->NextSiblingElement("LightSphere");
	}

	// Get Mesh Light
	element = root->FirstChildElement("Objects");
	element = element->FirstChildElement("LightMesh");
	while (element)
	{
		int material_id;

		// Shading Mode
		ShadingMode s_mode = ShadingMode::Flat;

		if (element->Attribute("shadingMode", "smooth") != NULL)
		{
			s_mode = ShadingMode::Smooth;
		}
		else if (element->Attribute("shadingMode", "flat") != NULL)
		{
			s_mode = ShadingMode::Flat;
		}
		else
		{
			s_mode = ShadingMode::Flat;
		}


		child = element->FirstChildElement("Radiance");
		stream << child->GetText() << std::endl;
		vec3 radiance;
		stream >> radiance.x >> radiance.y >> radiance.z;

		child = element->FirstChildElement("Material");
		stream << child->GetText() << std::endl;
		stream >> material_id;
		//material_id--;

		int texture_id = -1;
		child = element->FirstChildElement("Textures");

		raven::Texture* diffuse_texture = nullptr;
		raven::Texture* normal_texture = nullptr;
		raven::Texture* specular_texture = nullptr; // TODO: will be implemented

		if (child)
		{
			int diffuse_texture_id = -1;
			int normal_texture_id = -1;

			stream << child->GetText() << std::endl;
			stream >> diffuse_texture_id >> normal_texture_id;

			if (diffuse_texture_id > 0 && normal_texture_id > 0)
			{
				diffuse_texture = m_textures[diffuse_texture_id - 1];
				normal_texture = m_textures[normal_texture_id - 1];
			}

			if (diffuse_texture_id > 0 && normal_texture_id == -1)
			{
				if ((m_textures[diffuse_texture_id - 1]->m_is_normal_map) == false)
				{
					diffuse_texture = m_textures[diffuse_texture_id - 1];
				}
				else
				{
					normal_texture = m_textures[diffuse_texture_id - 1];
				}
			}
		}

		stream.clear();

		// Transformation

		child = element->FirstChildElement("Transformations");

		glm::mat4 M(1.0f);

		if (child) // calculate composite matrix
		{
			char _type;
			int _i;
			stream.clear();
			stream << child->GetText() << std::endl;

			while (!(stream >> _type).eof())
			{
				stream >> _i;
				_i--;

				if (_type == 't')
				{
					M = m_translations[_i].m_transformations * M;
				}
				if (_type == 's')
				{
					M = m_scalings[_i].m_transformations * M;
				}
				if (_type == 'r')
				{
					M = m_rotations[_i].m_transformations * M;
				}
			}
			stream.clear();
		}

		stream.clear();


		// Faces

		child = element->FirstChildElement("Faces");
		int vertexOffset = child->IntAttribute("vertexOffset", 0);

		const char* ply_file_name = child->Attribute("plyFile");

		char dir[100] = "src/Scenes/hw6/sponza/inputs/"; // TODO: get from args
		std::vector<Shape*> triangles;

		if (ply_file_name)
		{
			// Parse ply file TODO: write ply parser helper function
			// src/Scene/

			strcat(dir, ply_file_name);

			happly::PLYData ply_file(dir);

			std::vector<std::string> names = ply_file.getElementNames(); // for debug purpose, list all the names 

			std::vector<std::array<double, 3>> vertices = ply_file.getVertexPositions();
			std::vector<std::vector<size_t>> face = ply_file.getFaceIndices<size_t>();


			//TODO: add tex coords

			bool is_uv_exist = ply_file.getElement("vertex").hasProperty("u");
			std::vector<double> u_pos;
			std::vector<double> v_pos;


			// Get Vertices and UVs put them on m_vertices

			// Get Vertices
			int num_existing_vertices = m_vertices.size();

			for (int i = 0; i < vertices.size(); i++)
			{
				m_vertices.emplace_back(vec3(vertices[i][0], vertices[i][1], vertices[i][2]));
			}


			if (is_uv_exist)
			{
				u_pos = ply_file.getElement("vertex").getProperty<double>("u");
				v_pos = ply_file.getElement("vertex").getProperty<double>("v");

				std::vector<std::array<double, 2>> res(u_pos.size());
				for (size_t i = 0; i < res.size(); i++)
				{
					res[i][0] = u_pos[i];
					res[i][1] = v_pos[i];
				}

				std::vector<std::array<double, 2>> tex_coords = res;

				for (int i = 0; i < tex_coords.size(); i++)
				{
					vec2 uv = vec2(tex_coords[i][0], tex_coords[i][1]);
					m_vertices[i + num_existing_vertices].m_uv = uv;
					m_tex_coords.push_back(uv);
				}

			}
			for (size_t i = 0; i < face.size(); i++)
			{
				if (face[i].size() == 3) // For Triangle Faces
				{

					auto v1 = vertices[face[i][0]];
					auto v2 = vertices[face[i][1]];
					auto  v3 = vertices[face[i][2]];

					vec3 temp_v1(v1[0], v1[1], v1[2]);
					vec3 temp_v2(v2[0], v2[1], v2[2]);
					vec3 temp_v3(v3[0], v3[1], v3[2]);

					triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][1], num_existing_vertices + face[i][2], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));
				}

				if (face[i].size() == 4) // For Quad Faces
				{
					auto v1 = vertices[face[i][0]];
					auto v2 = vertices[face[i][1]];
					auto  v3 = vertices[face[i][2]];
					auto  v4 = vertices[face[i][3]];

					vec3 temp_v1(v1[0], v1[1], v1[2]);
					vec3 temp_v2(v2[0], v2[1], v2[2]);
					vec3 temp_v3(v3[0], v3[1], v3[2]);
					vec3 temp_v4(v4[0], v4[1], v4[2]);

					triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][1], num_existing_vertices + face[i][2], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));
					triangles.emplace_back(new Triangle(num_existing_vertices + face[i][0], num_existing_vertices + face[i][2], num_existing_vertices + face[i][3], s_mode, material_id, diffuse_texture, normal_texture, Transformations(M)));

				}
			}
		}
		else

		{
			stream << child->GetText() << std::endl;

			int v0_id, v1_id, v2_id;

			while (!(stream >> v0_id).eof())
			{
				stream >> v1_id >> v2_id;
				Triangle* new_triangle = new Triangle(v0_id - 1 + vertexOffset, v1_id - 1 + vertexOffset, v2_id - 1 + vertexOffset, s_mode, material_id, diffuse_texture, normal_texture, Transformations(M));

				if (s_mode == ShadingMode::Smooth)
				{
					m_vertices[v0_id - 1 + vertexOffset].add_vertex_normal(new_triangle->m_surface_normal);
					m_vertices[v1_id - 1 + vertexOffset].add_vertex_normal(new_triangle->m_surface_normal);
					m_vertices[v2_id - 1 + vertexOffset].add_vertex_normal(new_triangle->m_surface_normal);
				}

				// Find Degenerated Triangles
				if (glm::isnan(m_vertices[v0_id - 1 + vertexOffset].m_normal) == glm::bvec3(true) || glm::isnan(m_vertices[v1_id - 1 + vertexOffset].m_normal) == glm::bvec3(true) ||
					glm::isnan(m_vertices[v2_id - 1 + vertexOffset].m_normal) == glm::bvec3(true))
				{
					std::cerr << "Face 19 " << std::endl;
					std::cerr << "Normal " << new_triangle->m_surface_normal.x << " " << new_triangle->m_surface_normal.y << " " << new_triangle->m_surface_normal.z << std::endl;
					std::cerr << "Vertex v0 " << m_vertices[v0_id - 1 + vertexOffset].m_position.x << " " << m_vertices[v0_id - 1 + vertexOffset].m_position.y << " " << m_vertices[v0_id + vertexOffset - 1].m_position.z << std::endl;
					std::cerr << "Vertex v1 " << m_vertices[v1_id - 1 + vertexOffset].m_position.x << " " << m_vertices[v1_id - 1 + vertexOffset].m_position.y << " " << m_vertices[v1_id + vertexOffset - 1].m_position.z << std::endl;
					std::cerr << "Vertex v2 " << m_vertices[v2_id - 1 + vertexOffset].m_position.x << " " << m_vertices[v2_id - 1 + vertexOffset].m_position.y << " " << m_vertices[v2_id + vertexOffset - 1].m_position.z << std::endl;

					std::cerr << "Length v0 - v1 " << length(m_vertices[v0_id - 1 + vertexOffset].m_position - m_vertices[v1_id - 1 + vertexOffset].m_position) << std::endl;
					std::cerr << "Length v1 - v2 " << length(m_vertices[v1_id - 1 + vertexOffset].m_position - m_vertices[v2_id - 1 + vertexOffset].m_position) << std::endl;

				}


				triangles.emplace_back(new_triangle);
			}

			stream.clear();
		}

		MeshLight* mesh_light = new MeshLight(material_id, diffuse_texture, normal_texture, triangles, Transformations(M), radiance);

		m_lights.push_back(mesh_light);
		m_primitives.push_back(mesh_light);

		element = element->NextSiblingElement("LightMesh");
	}

	// For Smooth Shading
	// 
	//for (int v = 0; v < m_vertices.size(); v++)
	//{
	//	//m_vertices[v].m_normal = m_vertices[v].m_normal / m_vertices[v].m_num;
	//	if (length(m_vertices[v].m_normal) > 0.0f)
	//	{
	//		m_vertices[v].m_normal = normalize(m_vertices[v].m_normal);
	//	}
	//}



	m_bvh = BVH::create(m_primitives);

	//for (auto& vertex : m_vertices) // you need to normalize normals once
	//{
	//	/*vertex.m_normal = vertex.m_normal / vertex.m_num;
	//	vertex.m_normal = normalize(vertex.m_normal);*/
	//	vertex.normalize_vertex_normals();
	//}
	if (m_bvh)
	{
		std::cerr << "BVH is created!" << std::endl;
	}
	else
	{
		std::cerr << "BVH is not created!" << std::endl;
		throw std::runtime_error("BVH creating has error!");
	}
}

raven::Scene::~Scene()
{
	delete m_bvh;

	//for (size_t i = 0; m_primitives.size(); i++)
	//{
	//	delete[] m_primitives[i];
	//}
}
