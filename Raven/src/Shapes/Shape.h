#pragma once

#include "Core/HitRecord.h"
#include "Core/Ray.h"
#include "BBox.h"


using raven::Ray;
using raven::BBox;

/*
* Abstract Class for Shapes like Triangle, Sphere etc.
*
*/

class Shape
{
public:
	virtual bool hit(const Ray& ray, float t_min, float t_max, HitRecord& rec) const = 0;
	virtual bool shadow_hit(const Ray& ray, float t_min, float t_max, float time) const = 0;
	virtual int get_material_id() const = 0;
	virtual int get_texture_id() const = 0;
	virtual const BBox& get_bbox() const = 0;

	// TODO: add BBox, material_id since they are used in every primitives
};