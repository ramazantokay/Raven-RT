#include "ravenpch.h"
#include "Image.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "Utils/stb_image.h"

#define TINYEXR_IMPLEMENTATION
#include "Utils/tinyexr.h"

#include "Utils/lodepng.h"

namespace raven
{
	
	void Image::read_image(const std::string& filename)
	{
		// check file extension

		int dot_pos = filename.find_last_of('.');
		std::string extension = filename.substr(dot_pos + 1);

		if (extension == "exr")
		{
			read_from_exr(filename);
		}
		else
		{
			read_from_jpeg_or_png(filename);
		}

	}

	void Image::write_image(const std::string& filename, const Tonemap& Tonemap) const
	{
		// check file extension

		int dot_pos = filename.find_last_of('.');
		std::string extension = filename.substr(dot_pos + 1);
		std::string filename_no_extension = filename.substr(0, dot_pos);

		if (extension == "exr")
		{
			// write to exr
			write_to_exr(filename, Tonemap);
		}
		else
		{
			// write to png
			write_to_png(filename_no_extension + ".png", Tonemap);
		}

	}

	void Image::write_to_png(const std::string& filename, const Tonemap& tonemap) const
	{
		unsigned char* data = apply_tonemap(tonemap);

		if (data)
		{
			lodepng::encode(filename.c_str(), data, m_width, m_height);
		}
		else
		{
			std::cerr << "Error: Could not apply tonemap to image." << std::endl;
		}

	}

	void Image::write_to_exr(const std::string& filename, const Tonemap& Tonemap) const
	{

		//unsigned char* data = apply_tonemap(Tonemap);

		//// put data into vec3 format
		//std::vector<vec3> data_vec3(m_width * m_height, vec3());
		//int idx = 0;
		//for (int j = 0; j < m_height; j++)
		//{
		//	for (int i = 0; i < m_width; i++)
		//	{
		//		data_vec3[j * m_width + i] = vec3(data[idx], data[idx + 1], data[idx + 2]);
		//		idx += 4;
		//	}
		//}

		EXRHeader header;
		InitEXRHeader(&header);

		EXRImage image;
		InitEXRImage(&image);

		image.num_channels = 3;

		std::vector<float> images[3];
		images[0].resize(m_width * m_height);
		images[1].resize(m_width * m_height);
		images[2].resize(m_width * m_height);

		for (int i =0; i< m_width * m_height; i++)
		{
			vec3 pixel = m_pixels[i];
			images[0][i] = pixel.r;
			images[1][i] = pixel.g;
			images[2][i] = pixel.b;
		}

		float* image_ptr[3];
		image_ptr[0] = &(images[2].at(0)); // B
		image_ptr[1] = &(images[1].at(0)); // G
		image_ptr[2] = &(images[0].at(0)); // G

		image.images = (unsigned char**)image_ptr;
		image.width = m_width;
		image.height = m_height;

		header.num_channels = 3;
		header.channels = (EXRChannelInfo*)malloc(sizeof(EXRChannelInfo) * header.num_channels);

		strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
		strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
		strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

		header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
		header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
		for (int i = 0; i < header.num_channels; i++)
		{
			header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
			header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
		}

		const char* err = NULL;
		int ret = SaveEXRImageToFile(&image, &header, filename.c_str(), &err);
		if (ret != TINYEXR_SUCCESS)
		{
			fprintf(stderr, "Save EXR err: %s\n", err);
			FreeEXRErrorMessage(err); // free's buffer for an error message
		}

		free(header.channels);
		free(header.pixel_types);
		free(header.requested_pixel_types);
	}

	void Image::read_from_exr(const std::string& filename)
	{
		float* image;
		const char* err;

		int ret = LoadEXR(&image, &m_width, &m_height, filename.c_str(), &err);
		if (ret != TINYEXR_SUCCESS)
		{
			if (err)
			{
				fprintf(stderr, "ERR : %s\n", err);
				FreeEXRErrorMessage(err); 
			}
		}

		m_pixels.resize(m_width * m_height, vec3());

		for (int i = 0; i < m_width * m_height; i++)
		{
			m_pixels[i] = vec3(image[4 * i], image[4 * i + 1], image[4 * i + 2]);
		}

		free(image);
	}

	void Image::read_from_jpeg_or_png(const std::string& filename)
	{
		int channels;
		unsigned char* data = stbi_load(filename.c_str(), &m_width, &m_height, &channels, 3);

		if (data)
		{
			m_pixels.resize(m_width * m_height, vec3());

			for (int i = 0; i < m_width * m_height; i++)
			{
				m_pixels[i] = vec3(data[3 * i], data[3 * i + 1], data[3 * i + 2]);
			}

			stbi_image_free(data);
		}
		else
		{
			std::cerr << "Error: Could not load image " << filename << std::endl;
		}
	}

	unsigned char* Image::apply_tonemap(const Tonemap& tonemap) const
	{
		unsigned char* data = new unsigned char[m_width * m_height*4];
		int idx = 0;

		if (tonemap.m_type == TonemapType::None)
		{
			// no tonemap and clamp

			for (int j = 0; j < m_height; j++)
			{
				for (int i = 0; i < m_width; i++)
				{
					vec3 pixel = m_pixels[j * m_width + i];
					data[idx++] = clamp(pixel.r, 0, 255);
					data[idx++] = clamp(pixel.g, 0, 255);
					data[idx++] = clamp(pixel.b, 0, 255);
					data[idx++] = 255;
				}
			}
		}
		else if (tonemap.m_type == TonemapType::Reinhard_Photographic)
		{
			// TODO:

			// Reinhard Photographic

			int n = m_width * m_height;
			std::vector<float> world_luminance(n, 0.0f);
			std::vector<float> luminances(n, 0.0f);

			float lw_sum = 0.0f;

			for (int i=0; i < n; i++)
			{
				vec3 pixel = m_pixels[i];
				if (pixel.r < 0 || pixel.g < 0 || pixel.b < 0) {
					std::cout << "Negative pixel value detected at index " << i << ": "
						<< "r = " << pixel.r << ", g = " << pixel.g << ", b = " << pixel.b << std::endl;
				}
				//float luminance = 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722f * pixel.b;
				
				float luminance = std::max(0.0f, 0.2126f * pixel.r + 0.7152f * pixel.g + 0.0722f * pixel.b);

				world_luminance[i] = luminance;
				lw_sum += log(0.00001f + luminance);

				//lw_sum += log(std::max(0.00001f, luminance));

				//if (world_luminance[i] > 100.0f) {
				//	std::cout << "High luminance detected at index " << i << ": " << world_luminance[i] << std::endl;
				//}

			}

			float Lw = exp(lw_sum / n);

			for (int i = 0; i < n; i++)
			{
				luminances[i] = (tonemap.m_key_value * world_luminance[i]) / Lw;
			}

			float burn_ration = (100.0f - tonemap.m_burn) / 100.0f;
			int idx_lw = (int)(n * burn_ration) - 1;
			if (idx_lw < 0) idx_lw = 0;

			//std::nth_element(luminances.begin(), luminances.begin() + idx, luminances.end());

			// sort luminances and find LW

			std::vector<float> sorted_luminances = luminances;
			std::sort(sorted_luminances.begin(), sorted_luminances.end());
			float LW = sorted_luminances[idx_lw];

			std::vector<float> final_luminances(n, 0.0f);

			// TODO: check if this is correct
			for (int i = 0; i < n; i++)
			{
				//final_luminances[i] = luminances[i] / (1.0f + luminances[i] / (LW * LW));

				float L = luminances[i];
				float Ld = (L * (1.0f + (L / (LW * LW)))) / (1.0f + L);
				final_luminances[i] = Ld;


			/*	if (final_luminances[i] > 1.0f) {
					std::cout << "High final luminance at index " << i << ": " << final_luminances[i] << std::endl;
				}*/
			}


			for (int i = 0; i < n; i++)
			{
				vec3 pixel = m_pixels[i];
				
				// saturation
				float r = final_luminances[i] * pow((pixel.r / world_luminance[i]), tonemap.m_saturation);
				float g = final_luminances[i] * pow((pixel.g / world_luminance[i]), tonemap.m_saturation);
				float b = final_luminances[i] * pow((pixel.b / world_luminance[i]), tonemap.m_saturation);

				// gamma correction

				int g_a = (int)(pow(r, 1.0f / tonemap.m_gamma) * 255.f);
				int g_b = (int)(pow(g, 1.0f / tonemap.m_gamma) * 255.f);
				int g_c = (int)(pow(b, 1.0f / tonemap.m_gamma) * 255.f);

				data[idx++] = clamp(g_a, 0, 255);
				data[idx++] = clamp(g_b, 0, 255);
				data[idx++] = clamp(g_c, 0, 255);
				data[idx++] = 255;

			}

		}

		return data;
	}

}