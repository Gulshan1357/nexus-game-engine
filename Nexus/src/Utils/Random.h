#pragma once

#include <random>

class Random
{
public:
	static void Init()
	{
		m_randomEngine.seed(std::random_device()());
	}

	// Returns float between 0 and 1
	static float Float()
	{
		std::uniform_real_distribution<float> dist(0.0f, 1.0f);
		return dist(m_randomEngine);
	}

	// Returns float between min and max
	static float Float(const float min, const float max)
	{
		std::uniform_real_distribution<float> dist(min, max);
		return dist(m_randomEngine);
	}

	// Returns int between min and max
	static int Int(const int min, const int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_randomEngine);
	}

private:
	static std::mt19937 m_randomEngine;
};