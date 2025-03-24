#pragma once

#include "ravenpch.h"


namespace raven
{
	namespace utils
	{
		class RavenRandom
		{
		// Functions
		public:
			// Constructor
			RavenRandom(unsigned long long _seed = 7564231ULL)
			{
				m_seed = _seed;
				m_mult = 62089911ULL;
				m_llongMax = 4294967295ULL;
				m_floatMax = 4294967295.0f;
			}
			
			float operator()();
		
		// Variables
		public:
			unsigned long long m_seed;
			unsigned long long m_mult;
			unsigned long long m_llongMax;
			float m_floatMax;
		};
		
		inline float RavenRandom::operator()()
		{
			m_seed = m_mult * m_seed;
			return float(m_seed % m_llongMax) / m_floatMax;
		}
	}
}