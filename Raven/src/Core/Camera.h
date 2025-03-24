#pragma once

#include <string>

#include "Math/ONB.h"
#include "Core/Ray.h"
#include "Core/Tonemap.h"
#include "Math/glm/glm.hpp"

using glm::vec3;

namespace raven
{

	enum class RendererType
	{
		None,
		RayTracing,
		PathTracing
	};

	class Camera
	{
	// Functions
	public:
		Camera() = default;

		Camera(vec3 p, vec3 gaze, vec3 up, float near_distance, float focus_distance, float aperture_size,
			float l, float r, float t, float b, std::string name, int width, int height, int num_samples, Tonemap tmap, RendererType r_type,
			bool _nee, bool _is, bool _rr, int _s_factor) :
			m_position(p), m_gaze(gaze), m_up(up), m_near_distance(near_distance), m_focus_distance(focus_distance), m_aperture_size(aperture_size),
			m_r(r), m_l(l), m_t(t), m_b(b), m_image_name(name), m_image_width(width),
			m_image_height(height), m_num_samples(num_samples), m_tonemap(tmap), m_renderer_type(r_type),
			m_is_nee(_nee), m_is_importance_sampling(_is), m_is_russian_roulette(_rr), m_splitting_factor(_s_factor)
		{
			m_frame.init_from_wv(-gaze, up);
			m_top_left_corner = m_position - m_frame.w * m_near_distance + m_l * m_frame.u + m_t * m_frame.v;
			
		}

		Ray get_ray(int x, int y) const
		{
			float s_u = (m_r - m_l) * (float(x) + 0.5f) / float(m_image_width);
			float s_v = (m_t - m_b) * (float(y) + 0.5f) / float(m_image_height);

			vec3 s = m_top_left_corner + s_u * m_frame.u - s_v * m_frame.v;
			Ray r((m_position), normalize(s - m_position));
			return r;

		}

		Ray get_dof_ray(int x, int y, float dof_x, float dof_y) const
		{
			float s_u = (m_r - m_l) * (x + 0.5f) / m_image_width;
			float s_v = (m_t - m_b) * (y + 0.5f) / m_image_height;

			vec3 pixel_sample = m_top_left_corner + s_u * m_frame.u - s_v * m_frame.v;

			vec3 aperture_sample = m_position + m_aperture_size * (dof_x * m_frame.u + dof_y * m_frame.v);

			vec3 dir = normalize(pixel_sample - m_position);

			float t_focus_distance = m_focus_distance / dot(dir, m_gaze);

			Ray r((m_position), dir);

			vec3 point_on_focal_plane = r.at(t_focus_distance);
			vec3 d = normalize(point_on_focal_plane - aperture_sample);

			Ray primary_ray(aperture_sample, d);

			return primary_ray;
		}
		

	// Vars
	public:
		vec3 m_position, m_gaze, m_up, m_top_left_corner;
		float m_l, m_r, m_t, m_b;
		float m_near_distance, m_focus_distance, m_aperture_size;
		int m_image_width, m_image_height; // TODO: integrate with Image class, change parser
		int m_num_samples;

		raven::Tonemap m_tonemap;

		ONB m_frame; // camera orthonormal basis u = across, v = up, w = -gaze
		std::string m_image_name;
		RendererType m_renderer_type;
		bool m_is_nee;
		bool m_is_importance_sampling;
		bool m_is_russian_roulette;
		int m_splitting_factor;



	};
}