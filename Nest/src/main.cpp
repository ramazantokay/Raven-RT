#include "ravenpch.h"

#include "Core/HitRecord.h"
#include "Core/RavenArray.h"
#include "Core/RGB.h"
#include "Core/Image.h"
//#include "Shapes/Shape.h"
//#include "Shapes/Triangle.h"
//#include "Shapes/Sphere.h"
#include "Utils/Scene.h"
#include "Utils/lodepng.h"
#include "Core/Renderer.h"

#include "Math/glm/glm.hpp"

#include "Utils/RavenGlobals.h"

#define RAVEN_PROFILING 1
#include "Utils/Profiler.h"

#include <thread>

using std::cout;
using std::cerr;
using std::endl;

raven::Scene* raven_scene = nullptr;
Renderer* renderer = nullptr;


static void write_ppm(const char* filename, unsigned char* data, int width, int height)
{
	FILE* outfile;

	if ((outfile = fopen(filename, "w")) == NULL)
	{
		throw std::runtime_error("Error: The ppm file cannot be opened for writing.");
	}

	(void)fprintf(outfile, "P3\n%d %d\n255\n", width, height);

	unsigned char color;
	for (size_t j = 0, idx = 0; j < height; ++j)
	{
		for (size_t i = 0; i < width; ++i)
		{
			for (size_t c = 0; c < 3; ++c, ++idx)
			{
				color = data[idx];

				if (i == width - 1 && c == 2)
				{
					(void)fprintf(outfile, "%d", color);
				}
				else
				{
					(void)fprintf(outfile, "%d ", color);
				}
			}
		}

		(void)fprintf(outfile, "\n");
	}

	(void)fclose(outfile);
}


void init_scene(const char* name)
{
	raven_scene = new raven::Scene();
	raven_scene->parse_scene(name);
	std::cerr << "Scene parsing OK!" << std::endl;
}

int main(int argc, char* argv[]) {


	try
	{
		std::string scene_name(argv[1]);

		size_t last_slash = scene_name.find_last_of('/');

		size_t last_dot = scene_name.find_last_of('.');

		if (last_slash != std::string::npos)
		{
			if (last_dot != std::string::npos)
				scene_name = scene_name.substr(last_slash + 1, last_dot - last_slash - 1);
		}
		else
		{
			if (last_dot != std::string::npos)
				scene_name = scene_name.substr(0, last_dot);
		}

		scene_name += ".json"; // for benchmark purpose

		RAVEN_PROFILE_BEGIN_SESSION("Raven Ray Tracer Initiliazed", scene_name.c_str());

		std::cerr << argv[1] << std::endl;

		{
			RAVEN_PROFILE_SCOPE("Scene Init");
			init_scene(argv[1]);
		}

		{
			RAVEN_PROFILE_SCOPE("Renderer Init");

			renderer = new Renderer(raven_scene);
			std::cerr << "Renderer OK!" << std::endl;
		}

		int num_camera = int(raven_scene->m_cameras.size());

		//const int num_threads = std::max(1, static_cast<int>(std::thread::hardware_concurrency()));
		const int num_threads = 16;

		std::cerr << "Raven will use: " << num_threads << " threads" << std::endl;

		for (int i = 0; i < num_camera; i++)
		{
			int w = raven_scene->m_cameras[i].m_image_width;
			int h = raven_scene->m_cameras[i].m_image_height;

			std::vector<vec3> pixels_vec(w * h, vec3(0.0f));

			{
				RAVEN_PROFILE_SCOPE("Rendering Part!");
				
				//renderer->render(i, pixels_vec, 0, num_threads);
				
				std::vector<std::thread> threads;

				for (int t = 0; t < num_threads; t++)
				{
					threads.emplace_back(std::thread(&Renderer::render, renderer, i, std::ref(pixels_vec), t, num_threads));
				}

				for (auto& thread : threads)
				{
					thread.join();
				}
			}

			std::cerr << "Rendering is Finished!" << std::endl;
			std::cerr << "Saving has Started!" << std::endl;

			{
				RAVEN_PROFILE_SCOPE("Saving to PPM");

				raven::Image image(w, h, (pixels_vec) );
				image.write_image(raven_scene->m_cameras[i].m_image_name, raven_scene->m_cameras[i].m_tonemap);
				//image.write_to_png((raven_scene->m_cameras[i].m_image_name).c_str(), raven_scene->m_cameras[i].m_tonemap);
			}
		}
		RAVEN_PROFILE_END_SESSION();
		delete raven_scene;
		std::cerr << "Saving has Finished!" << std::endl;

	}
	catch (std::exception& e) // TODO: create RavenException class
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}



	return 0;
}