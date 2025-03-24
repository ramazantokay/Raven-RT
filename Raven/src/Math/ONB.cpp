#include "ravenpch.h"
#include "ONB.h"
#define ONB_EPSILON 0.01f

using raven::ONB;

using glm::normalize;
using glm::cross;
using glm::length2;

void ONB::init_from_u(const vec3& u)
{
    vec3 n(1.0f, 0.0f, 0.0f);
    vec3 m(0.0f, 1.0f, 0.0f);

    this->u = normalize(u);
    this->v = cross(this->u, n);

    if (glm::length(this->v) < ONB_EPSILON)
        this->v = cross(this->u, m);

    this->w = cross(this->u, this->v);
}

void ONB::init_from_v(const vec3& v)
{
	vec3 n(1.0f, 0.0f, 0.0f);
	vec3 m(0.0f, 1.0f, 0.0f);

	this->v = normalize(v);
	this->u = cross(this->v, n);

	if (glm::length2(this->u)< ONB_EPSILON)
		this->u = cross(this->v, m);

	this->w = cross(this->u, this->v);
}

void ONB::init_from_w(const vec3& w)
{
	vec3 n(1.0f, 0.0f, 0.0f);
	vec3 m(0.0f, 1.0f, 0.0f);

	this->w = normalize(w);
	this->u = cross(this->w, n);

	if (glm::length(this->u) < ONB_EPSILON)
		this->u = cross(this->w, m);

	this->v = cross(this->w, this->u);
}

void ONB::init_from_uv(const vec3& u, const vec3& v)
{
	this->u = normalize(u);
	this->w = normalize(cross(u,v));
	this->v = cross(this->w, this->u);
}

void ONB::init_from_vu(const vec3& v, const vec3& u)
{
	this->v = normalize(v);
	this->w = normalize(cross(u, v));
	this->u = cross(this->v, this->w);
}

void ONB::init_from_uw(const vec3& u, const vec3& w)
{
	this->u = normalize(u);
	this->v = normalize(cross(w, u));
	this->w = cross(this->u, this->v);
}

void ONB::init_from_wu(const vec3& w, const vec3& u)
{
	this->w = normalize(w);
	this->v = normalize(cross(w, u));
	this->u = cross(this->v, this->w);
}

void ONB::init_from_vw(const vec3& v, const vec3& w)
{
	this->v = normalize(v);
	this->u = normalize(cross(v, w));
	this->w = cross(this->u, this->v);
}

void ONB::init_from_wv(const vec3& w, const vec3& v)
{
	this->w = normalize(w);
	this->u = normalize(cross(v, w));
	this->v = cross(this->w, this->u);
}

void ONB::init_from_wr(const vec3& w_r) // for glossy
{
	vec3 temp_wr = w_r;
	int min_index = get_min_element_index_in_glm_vec3(temp_wr);
	temp_wr[min_index] = 1.0f;

	this->u = normalize(cross(w_r, temp_wr));
	this->v = cross(w_r, u);
	this->w = w_r;
}

//std::istream& operator>>(std::istream& is, ONB& onb)
//{
//	vec3 n_u, n_v, n_w;
//	is >> n_u >> n_v >> n_w;
//	onb.init_from_uv(n_u, n_v);
//
//	return is;
//}

std::ostream& operator<<(std::ostream& out, ONB& onb)
{
	out << glm::to_string(onb.u) << "\n" << glm::to_string(onb.v) << "\n" << glm::to_string(onb.w) << "\n";
	return out;
}

bool operator==(const ONB& o1, const ONB& o2)
{
    return false;
}
