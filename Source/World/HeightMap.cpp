#include "HeightMap.h"
#include "Math/Vectors/Vec2.h"

namespace Minecraft {
	namespace World {

		using namespace Engine;
		using namespace Math;
		using namespace noise;


		HeightMap::HeightMap(const IVec2& destinationSize, const Vec2& bounds, const NoiseModuleData& data) {
			// Creating the noise module for the height map
			module::Perlin perlinModule;
			// Adjusting noise module parameters for desired map layout
			perlinModule.SetOctaveCount(data.octaveCount);
			perlinModule.SetFrequency(data.frequency);
			perlinModule.SetPersistence(data.persistence);
			// Using a map builder object to pre generate the height map
			utils::NoiseMapBuilderPlane mapBuilder;
			mapBuilder.SetSourceModule(perlinModule);
			mapBuilder.SetDestNoiseMap(heightMap);
			mapBuilder.SetDestSize(destinationSize.x, destinationSize.y);
			mapBuilder.SetBounds(0.0, double(bounds.x), 0.0, double(bounds.y));
			mapBuilder.Build();
		}

		float HeightMap::operator[](const IVec2& position) const {
			return heightMap.GetValue(position.x, position.y);
		}
	}
}