#pragma once

#include <iostream>

// TODO: don't forget to clamp final color values
namespace raven
{
	class rgb
	{
	public:
		// Constructors
		rgb() : m_r(0.f), m_g(0.f), m_b(0.f) {}
		rgb(float _r, float _g, float _b);
		rgb(const rgb& original) { *this = original; }

		// Assignment operators
		rgb& operator=(const rgb& rhs);
		rgb& operator+=(const rgb& rhs);
		rgb& operator*=(const rgb& rhs);
		rgb& operator/=(const rgb& rhs);
		rgb& operator*=(float rhs);
		rgb& operator/=(float rhs);

		// Unary operators
		rgb operator+() const { return *this; }
		rgb operator-()const { return rgb(-m_r, -m_g, -m_b); }

		void clamp();

		unsigned char clamp_r() const;
		unsigned char clamp_g() const;
		unsigned char clamp_b() const;

		// IO operator
		friend std::ostream& operator<<(std::ostream& out, const rgb& _rgb);

		// Arithmetic operators
		friend rgb operator+(const rgb& color1, const rgb& color2);
		friend rgb operator*(const rgb& color1, const rgb& color2);
		friend rgb operator/(const rgb& color1, const rgb& color2);

		friend rgb operator*(float f, const rgb& color);
		friend rgb operator*(const rgb& color, float f);
		friend rgb operator/(const rgb& color, float f);

		// getter for rgb values
		// TODO: , I don't know if we need it since the values are public, check it out
		float r() const { return m_r; }
		float g() const { return m_g; }
		float b() const { return m_b; }

	public:
		float m_r, m_g, m_b;
	};

	inline rgb::rgb(float _r, float _g, float _b) : m_r(_r), m_g(_g), m_b(_b) {}

	inline rgb& rgb::operator=(const rgb& rhs)
	{
		m_r = rhs.m_r;
		m_g = rhs.m_g;
		m_b = rhs.m_b;
		return *this;
	}
	inline rgb& rgb::operator+=(const rgb& rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline rgb& rgb::operator*=(float rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline rgb& rgb::operator/=(float rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline rgb& rgb::operator*=(const rgb& rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline rgb& rgb::operator/=(const rgb& rhs)
	{
		*this = *this / rhs;
		return *this;
	}

	inline void rgb::clamp()
	{
		// TODO: optimize it, check it out how can we avoid if branches
		/*if (m_r > 1.0) m_r = 1.0f;
		if (m_g > 1.0) m_g = 1.0f;
		if (m_b > 1.0) m_b = 1.0f;

		if (m_r < 0.0) m_r = 0.0f;
		if (m_g < 0.0) m_g = 0.0f;
		if (m_b < 0.0) m_b = 0.0f;*/
	}

	inline unsigned char rgb::clamp_r() const
	{
		unsigned int c = (unsigned int)(256.0 * m_r);
		if (c > 255) c = 255;
		return (unsigned char)c;
	}

	inline unsigned char rgb::clamp_g() const
	{
		unsigned int c = (unsigned int)(256.0 * m_g);
		if (c > 255) c = 255;
		return (unsigned char)c;
	}

	inline unsigned char rgb::clamp_b() const
	{
		unsigned int c = (unsigned int)(256.0 * m_b);
		if (c > 255) c = 255;
		return (unsigned char)c;
	}

	inline std::ostream& operator<<(std::ostream& out, const rgb& _rgb)
	{
		out << _rgb.m_r << " " << _rgb.m_g << " " << _rgb.m_b << " \n";
		return out;
	}

	// friend functions, operators without =

	inline rgb operator*(const rgb& color, float f)
	{
		return rgb(color.m_r * f, color.m_g * f, color.m_b * f);
	}

	inline rgb operator*(float f, const rgb& color)
	{
		return rgb(color.m_r * f, color.m_g * f, color.m_b * f);
	}

	inline rgb operator/(const rgb& color, float f)
	{
		return rgb(color.m_r / f, color.m_g / f, color.m_b / f);
	}

	inline rgb operator*(const rgb& color1, const rgb& color2)
	{
		return rgb(color1.m_r * color2.m_r, color1.m_g * color2.m_g, color1.m_b * color2.m_b);
	}

	inline rgb operator/(const rgb& color1, const rgb& color2)
	{
		return rgb(color1.m_r / color2.m_r, color1.m_g / color2.m_g, color1.m_b / color2.m_b);
	}

	inline rgb operator+(const rgb& color1, const rgb& color2)
	{
		return rgb(color1.m_r + color2.m_r, color1.m_g + color2.m_g, color1.m_b + color2.m_b);
	}
}
