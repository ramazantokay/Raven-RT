#include "ravenpch.h"
#include "PerlinNoise.h"
#include <random>
#include <chrono>

raven::PerlinNoise::PerlinNoise()
{
	m_G[0] = glm::vec3(1.0f, 1.0f, 0.0f);
	m_G[1] = glm::vec3(-1.0f, 1.0f, 0.0f);
	m_G[2] = glm::vec3(1.0f, -1.0f, 0.0f);
	m_G[3] = glm::vec3(-1.0f, -1.0f, 0.0f);

	m_G[4] = glm::vec3(1.0f, 0.0f, 1.0f);
	m_G[5] = glm::vec3(-1.0f, 0.0f, 1.0f);
	m_G[6] = glm::vec3(1.0f, 0.0f, -1.0f);
	m_G[7] = glm::vec3(-1.0f, 0.0f, -1.0f);

	m_G[8] = glm::vec3(0.0f, 1.0f, 1.0f);
	m_G[9] = glm::vec3(0.0f, -1.0f, 1.0f);
	m_G[10] = glm::vec3(0.0f, 1.0f, -1.0f);
	m_G[11] = glm::vec3(0.0f, -1.0f, -1.0f);

	m_G[12] = glm::vec3(1.0f, 1.0f, 0.0f);
	m_G[13] = glm::vec3(-1.0f, 1, 0.0f);
	m_G[14] = glm::vec3(0.0f, -1.0f, 1.0f);
	m_G[15] = glm::vec3(0.0f, -1.0f, -1.0f);

	for (int i = 0; i < 16; i++)
	{
		m_P.push_back(i);
	}

	std::mt19937 random_generator;
	random_generator.seed(std::chrono::system_clock::now().time_since_epoch().count());
	std::shuffle(m_P.begin(), m_P.end(), random_generator);
}

float raven::PerlinNoise::get_noise(const glm::vec3& pos) const
{
	float noise = 0.0f;

	// Compute lattice points surrounding point p.
	int x = floor(pos.x);
	int y = floor(pos.y);
	int z = floor(pos.z);

	std::vector<glm::ivec3> lattice_points;
	lattice_points.push_back(glm::ivec3(x, y, z));
	lattice_points.push_back(glm::ivec3(x, y, z + 1));
	lattice_points.push_back(glm::ivec3(x, y + 1, z));
	lattice_points.push_back(glm::ivec3(x, y + 1, z + 1));
	lattice_points.push_back(glm::ivec3(x + 1, y, z));
	lattice_points.push_back(glm::ivec3(x + 1, y, z + 1));
	lattice_points.push_back(glm::ivec3(x + 1, y + 1, z));
	lattice_points.push_back(glm::ivec3(x + 1, y + 1, z + 1));

	for (int n = 0; n < lattice_points.size(); n++)
	{
		glm::ivec3 lattice = lattice_points[n];
		int i = lattice.x;
		int j = lattice.y;
		int k = lattice.z;

		glm::vec3 e = get_gradient_vector(i, j, k);
		glm::vec3 v = pos - glm::vec3((float)i, (float)j, (float)k);

		noise += glm::dot(e,v) * weight(v.x) * weight(v.y) * weight(v.z);
	}

	return noise;
}

float raven::PerlinNoise::weight(float x) const
{
	x = fabs(x);
	return -6 * pow(x, 5) + 15 * pow(x, 4) - 10 * pow(x, 3) + 1;
}

float raven::PerlinNoise::phi(int i) const
{
	i = i % 16;
	if (i < 0)
		i += 16;

	return m_P[i];
}

glm::vec3 raven::PerlinNoise::get_gradient_vector(int i, int j, int k) const
{
	int idx = phi(i + phi(j + phi(k)));
	return m_G[idx];
}
