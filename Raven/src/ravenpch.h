#pragma once

#include <iostream>
#include <memory>
#include <utility>
#include <functional>
#include <cmath>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <stdexcept>


#ifdef RAVEN_PLATFORM_WINDOWS
	#define NOMINMAX
	#include <Windows.h>
#endif

#include <algorithm>

const float infinity = std::numeric_limits<float>::infinity();

const float raven_infinity = std::numeric_limits<float>::infinity();

#define RAVEN_PI 3.141592653f
