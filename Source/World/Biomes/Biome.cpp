#include "Biome.h"

namespace Minecraft {
	namespace World {

		using namespace Engine;
		using namespace Math;

		const Vec3 Biome::bottomBlockTexture = Vec3(3.0f);

		// The data used by a biome class instance
		struct BiomeData {
			bool hasTrees;
			bool hasLeaves;

			Engine::Math::Vec3 topBlockTexture;
			Engine::Math::Vec3 midBlockTexture;
			Engine::Math::Vec3 treeTexture;
			Engine::Math::Vec3 leafTexture;
		};

		// Retrieves the biome data for the given biome type
		static BiomeData getBiomeData(BiomeType type) {
			BiomeData data;
			if (type == BiomeType::FOREST) {
				data.hasTrees = true;
				data.hasLeaves = true;
				data.topBlockTexture = Vec3(1.0f, 2.0f, 0.0f);
				data.midBlockTexture = Vec3(2.0f);
				data.treeTexture = Vec3(4.0f, 5.0f, 5.0f);
				data.leafTexture = Vec3(6.0f);
			}
			if (type == BiomeType::DESERT) {
				data.hasTrees = true;
				data.hasLeaves = false;
				data.topBlockTexture = Vec3(7.0f);
				data.midBlockTexture = Vec3(7.0f);
				data.treeTexture = Vec3(9.0f);
				// No leaves in a desert biome
				data.leafTexture = Vec3(0.0f);
			}
			return data;
		}

		Biome::Biome(BiomeType type) {
			initBiome(type);
		}

		Biome::~Biome() {

		}

		void Biome::initBiome(BiomeType type) {
			BiomeData data = getBiomeData(type);
			trees = data.hasTrees;
			leaves = data.hasLeaves;
			topBlockTexture = data.topBlockTexture;
			midBlockTexture = data.midBlockTexture;
			treeTexture = data.treeTexture;
			leafTexture = data.leafTexture;
		}

	}
}