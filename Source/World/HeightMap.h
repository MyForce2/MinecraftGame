#pragma once

#include "noiseutils.h"
#include "Math/Vectors/IVec2.h"


namespace Minecraft {
	namespace World {

		/*
		A struct containing the basic data required for a noise module
		*/
		struct NoiseModuleData {
			int octaveCount;
			double frequency;
			double persistence;

			NoiseModuleData(int octaveCount, double frequency, double presistence) : octaveCount(octaveCount),
				frequency(frequency), persistence(persistence) {

			}
		};

		/*
		A class used to generate a height map for the terrain using a noise algorithm
		*/
		class HeightMap {
		public:
			utils::NoiseMap heightMap;

		public:
			HeightMap(const Engine::Math::IVec2& destinationSize, const Engine::Math::Vec2& bounds, const NoiseModuleData& data);

			float operator[](const Engine::Math::IVec2& position) const;

		};
	}
}