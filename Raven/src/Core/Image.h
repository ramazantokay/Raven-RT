#pragma once

#include "ravenpch.h"

#include "Math/glm/glm.hpp"
#include "Core/Tonemap.h"

using glm::vec3;


namespace raven
{
	class Image
	{

	// Constructors
	public:
		Image() : m_width(0), m_height(0) {}
		Image(int width, int height) : m_width(width), m_height(height) { m_pixels.resize(width * height, vec3()); }
		Image(int width, int height, const std::vector<vec3>& color) : m_width(width), m_height(height) { m_pixels = color; }
		Image(const std::string& filename) { read_image(filename); }




	// Helper functions
	public:
		vec3 get_pixel(int x, int y) const { return m_pixels[y * m_width + x]; }
		void read_image(const std::string& filename);
		void write_image(const std::string& filename, const Tonemap& Tonemap) const;

		int clamp(int value, int min, int max) const { return std::max(min, std::min(value, max)); }

		void write_to_png(const std::string& filename, const Tonemap& tonemap) const;
		void write_to_exr(const std::string& filename, const Tonemap& Tonemap) const;
		void read_from_exr(const std::string& filename);
		void read_from_jpeg_or_png(const std::string& filename);

		unsigned char* apply_tonemap(const Tonemap& tonemap) const;

	public:
		int m_width;
		int m_height;
		std::vector<vec3> m_pixels;

	};
}