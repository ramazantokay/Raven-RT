#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "Math/glm/glm.hpp"
#include "Math/glm/ext.hpp"
#include "Math/glm/gtx/string_cast.hpp"
#include "Math/glm/gtx/norm.hpp"
#include "Math/Helper.h"

using glm::vec3;

namespace raven
{
	class ONB
	{
	public:
		// Constructors
		ONB() : u(0.f), v(0.f), w(0.f) {} // TODO: will be checked
		ONB(const vec3& _u, const vec3& _v, const vec3& _w) : u(_u), v(_v), w(_w) { }


	public:

		// init functions
		void init_from_u(const vec3& u);
		void init_from_v(const vec3& v);
		void init_from_w(const vec3& w);

		void init_from_uv(const vec3& u, const vec3& v);
		void init_from_vu(const vec3& v, const vec3& u);

		void init_from_uw(const vec3& u, const vec3& w);
		void init_from_wu(const vec3& w, const vec3& u);

		void init_from_vw(const vec3& v, const vec3& w);
		void init_from_wv(const vec3& w, const vec3& v);

		void init_from_wr(const vec3& w_r); // for glossy
		// setter
		void set(const vec3& _u, const vec3& _v, const vec3& _w)
		{
			u = _u;
			v = _v;
			w = _w;
		}

		// IO operators
		//friend std::istream& operator>>(std::istream& is, ONB& onb);
		friend std::ostream& operator<<(std::ostream& out, ONB& onb);

		// Boolean operators
		friend bool operator==(const ONB& o1, const ONB& o2);

	public:
		vec3 u, v, w;
	};

}
