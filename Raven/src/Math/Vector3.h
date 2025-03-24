#pragma once 

#include <math.h>
#include <iostream>

namespace raven
{
	class Vector3
	{
	public:
		// TODO: copy constructor, move constructor, move assignment, copy assignment
		// TODO: will be trying to implement SIMD intrinsic
		// Constructors
		Vector3() : x(0.f), y(0.f), z(0.f) {} // TODO: delete or default 
		Vector3(float v) : x(v), y(v), z(v) {}
		Vector3(float _x, float _y, float _z);
		Vector3(const Vector3& v) { *this = v; }

		// Unary operators
		const Vector3& operator+() const; // TODO: will be check if it is correct. Is it extra? Do I need this operator?
		Vector3 operator-() const;

		// Boolean operators
		friend bool operator==(const Vector3& v1, const Vector3& v2);
		friend bool operator!=(const Vector3& v1, const Vector3& v2);
	
		// Arithmetic Operators 
		friend Vector3 operator+(const Vector3& v1, const Vector3& v2);
		friend Vector3 operator-(const Vector3& v1, const Vector3& v2);
		friend Vector3 operator*(const Vector3& v1, const Vector3& v2);
		friend Vector3 operator/(const Vector3& v1, const Vector3& v2);

		friend Vector3 operator/(const Vector3& v, float c);
		friend Vector3 operator/(float c, const Vector3& v);
		friend Vector3 operator*(const Vector3& v, float c);
		friend Vector3 operator*(float c, const Vector3& v);

		// Assignment operators
		Vector3& operator=(const Vector3& rhs);

		Vector3& operator+=(const Vector3& rhs);
		Vector3& operator-=(const Vector3& rhs);
		Vector3& operator*=(const Vector3& rhs);
		Vector3& operator/=(const Vector3& rhs);

		Vector3& operator+=(float c);
		Vector3& operator-=(float c);
		Vector3& operator*=(float c);
		Vector3& operator/=(float c);

		// Math operations
		friend Vector3 unit_vector(const Vector3& v);
		friend float dot(const Vector3& v1, const Vector3& v2);
		friend Vector3 cross(const Vector3& v1, const Vector3& v2);
		friend Vector3 min_vec(const Vector3& v1, const Vector3& v2);
		friend Vector3 max_vec(const Vector3& v1, const Vector3& v2);
		friend float triple_product(const Vector3& v1, const Vector3& v2, const Vector3& v3);

	public:
		// helper functions
		void make_unit_vector();
		float length() const;
		float lenght_squared() const;
		float min_component() const;
		float max_component() const;
		float min_abs_component() const;
		float max_abs_component() const;

	public:
		float x, y, z;
	};


	// Constructor
	inline Vector3::Vector3(float _x, float _y, float _z)
	{
		this->x = _x;
		this->y = _y;
		this->z = _z;
	}

	// Unary Operators
	inline const Vector3& Vector3::operator+() const
	{
		return *this;
	}

	inline Vector3 Vector3::operator-() const
	{
		return Vector3(-this->x, -this->y, -this->z);
	}

	// Boolean operators
	inline bool operator==(const Vector3& v1, const Vector3& v2)
	{
		if (v1.x != v2.x) return false;
		if (v1.y != v2.y) return false;
		if (v1.z != v2.z) return false;
		return true;
	}

	inline bool operator!=(const Vector3& v1, const Vector3& v2)
	{
		return !(v1 == v2);
	}

	// IO Operators

	inline std::ostream& operator<<(std::ostream& os, const Vector3& v)
	{
		return os << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	}

	inline std::istream& operator>>(std::istream& is, Vector3& v)
	{
		float temp;
		is >> temp;
		v.x = temp;
		is >> temp;
		v.y = temp;
		is >> temp;
		v.z = temp;

		return is;
	}

	// Arithmetic Operators
	inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	inline Vector3 operator*(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}

	inline Vector3 operator/(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x / v2.x, v1.y / v2.y, v1.z / v2.z);
	}

	inline Vector3 operator/(const Vector3& v, float c)
	{
		return Vector3(v.x / c, v.y / c, v.z / c);
	}

	inline Vector3 operator/(float c, const Vector3& v)
	{
		return Vector3(c / v.x, c / v.y, c / v.z);
	}

	inline Vector3 operator*(const Vector3& v, float c)
	{
		return Vector3(v.x * c, v.y * c, v.z * c);
	}

	inline Vector3 operator*(float c, const Vector3& v)
	{
		return Vector3(v.x * c, v.y * c, v.z * c);
	}

	inline Vector3 unit_vector(const Vector3& v)
	{
		return v / v.length();
	}

	inline float dot(const Vector3& v1, const Vector3& v2)
	{
		return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
	}

	inline Vector3 cross(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(
			v1.y * v2.z - v1.z * v2.y,
			v1.z * v2.x - v1.x * v2.z,
			v1.x * v2.y - v1.y * v2.x
		);
	}

	inline Vector3 min_vec(const Vector3& v1, const Vector3& v2)
	{
		Vector3 temp(v1);
		if (v2.x < v1.x) temp.x = v2.x;
		if (v2.y < v1.y) temp.y = v2.y;
		if (v2.z < v1.z) temp.z = v2.z;
		return temp;
	}

	inline Vector3 max_vec(const Vector3& v1, const Vector3& v2)
	{
		Vector3 temp(v1);
		if (v2.x > v1.x) temp.x = v2.x;
		if (v2.y > v1.y) temp.y = v2.y;
		if (v2.z > v1.z) temp.z = v2.z;
		return temp;
	}

	inline float triple_product(const Vector3& v1, const Vector3& v2, const Vector3& v3)
	{
		return dot(cross(v1, v2), v3);
	}


	// Assignment Operators
	inline Vector3& Vector3::operator=(const Vector3& rhs)
	{
		this->x = rhs.x;
		this->y = rhs.y;
		this->z = rhs.z;
		return *this;
	}

	inline Vector3& Vector3::operator+=(const Vector3& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	inline Vector3& Vector3::operator-=(const Vector3& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	inline Vector3& Vector3::operator*=(const Vector3& rhs)
	{
		this->x *= rhs.x;
		this->y *= rhs.y;
		this->z *= rhs.z;
		return *this;
	}

	inline Vector3& Vector3::operator/=(const Vector3& rhs)
	{
		this->x /= rhs.x;
		this->y /= rhs.y;
		this->z /= rhs.z;
		return *this;
	}

	inline Vector3& Vector3::operator+=(float c)
	{
		this->x += c;
		this->y += c;
		this->z += c;
		return *this;
	}
	inline Vector3& Vector3::operator-=(float c)
	{
		this->x -= c;
		this->y -= c;
		this->z -= c;
		return *this;
	}
	inline Vector3& Vector3::operator*=(float c)
	{
		this->x *= c;
		this->y *= c;
		this->z *= c;
		return *this;
	}

	inline Vector3& Vector3::operator/=(float c)
	{
		this->x /= c;
		this->y /= c;
		this->z /= c;
		return *this;
	}


	// Helper Functions
	inline float Vector3::length() const
	{
		return std::sqrt(this->x * this->x + this->y * this->y + this->z * this->z);
	}

	inline float Vector3::lenght_squared() const
	{
		return (this->x * this->x + this->y * this->y + this->z * this->z);
	}

	inline void Vector3::make_unit_vector()
	{
		*this = *this / (*this).length();
	}

	inline float Vector3::min_component() const
	{
		float temp = this->x;
		if (this->y < temp) temp = this->y;
		if (this->z < temp) temp = this->z;

		return temp;
	}

	inline float Vector3::max_component() const
	{
		float temp = this->x;
		if (this->y > temp) temp = this->y;
		if (this->z > temp) temp = this->z;

		return temp;
	}

	inline float Vector3::max_abs_component() const
	{
		float temp = std::fabs(this->x);
		if (std::fabs(this->y) > temp) temp = std::fabs(this->y);
		if (std::fabs(this->z > temp)) temp = std::fabs(this->z);

		return temp;
	}

	inline float Vector3::min_abs_component() const
	{
		float temp = std::fabs(this->x);
		if (std::fabs(this->y) < temp) temp = std::fabs(this->y);
		if (std::fabs(this->z < temp)) temp = std::fabs(this->z);

		return temp;
	}
}