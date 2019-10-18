#pragma once

#include <vector>

namespace Utils
{
	class NoiseGenerator
	{
	public:
		NoiseGenerator();

		double noise(double x, double y, double z) const;

	private:
		double fade(double t) const;
		double grad(int hash, double x, double y, double z) const;
		double lerp(double t, double a, double b) const;

		std::vector<int32_t> _permutation;
	};
}