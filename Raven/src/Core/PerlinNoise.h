#pragma once

#include "ravenpch.h"
#include "Math/glm/glm.hpp"

namespace raven

{


	enum class NoiseType
	{
		None,
		Abs,
		Linear
	};

	class PerlinNoise
	{
	public:
		PerlinNoise();

		float get_noise(const glm::vec3& pos) const;
		float weight(float x) const;
		float phi(int i) const;
		glm::vec3 get_gradient_vector(int i, int j, int k) const;

	public:
		glm::vec3 m_G[16];
		std::vector<int> m_P;


	};
}