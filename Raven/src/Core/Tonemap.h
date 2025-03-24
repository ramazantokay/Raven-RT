#pragma once

#include "ravenpch.h"

namespace raven
{

	enum class TonemapType
	{
		None,
		Reinhard_Photographic,
	};

	class Tonemap
	{
	public:
		Tonemap() :
			m_key_value(0.18f), m_burn(0.0f), m_saturation(1.0f), m_gamma(2.2f), m_type(TonemapType::None) {}

		Tonemap(std::string& tmo, float key_value, float burn, float saturation, float gamma) :
			m_key_value(key_value), m_burn(burn), m_saturation(saturation), m_gamma(gamma)
		{
			if (tmo == "Reinhard_Photographic")
				m_type = TonemapType::Reinhard_Photographic;
			else
				m_type = TonemapType::None;
		}

	public:
		float m_key_value;
		float m_burn;
		float m_saturation;
		float m_gamma;
		TonemapType m_type;
	};
}
