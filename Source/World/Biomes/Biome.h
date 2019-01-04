#pragma once

#include "Math/Vectors/Vec3.h"

namespace Minecraft {
	namespace World {

		enum class BiomeType {
			FOREST, DESERT
		};

		class Biome {
		private:
			// Are there trees in this biome
			bool trees;
			// Do the trees in this biome have leaves
			bool leaves;

			// The textures of this biome top block
			Engine::Math::Vec3 topBlockTexture;
			// The textures of this biome middle block
			Engine::Math::Vec3 midBlockTexture;
			// The textures of this biome tree
			Engine::Math::Vec3 treeTexture;
			// The textures of this biome leaves
			Engine::Math::Vec3 leafTexture;

			// The textures of the bottom block (Always stone)
			static const Engine::Math::Vec3 bottomBlockTexture;

		public:
			Biome(BiomeType type);
			~Biome();

			inline const Engine::Math::Vec3& getTopBlockTexture() const { return topBlockTexture; }
			inline const Engine::Math::Vec3& getMidBlockTexture() const { return midBlockTexture; }
			inline const Engine::Math::Vec3& getTreeTexture() const { return treeTexture; }
			inline const Engine::Math::Vec3& getLeafTexture() const { return leafTexture; }
			inline const bool hasTrees() const { return trees; }
			inline const bool hasLeaves() const { return leaves; }

			inline static const Engine::Math::Vec3& getBottomBlockTexture() { return bottomBlockTexture; }


		private:
			void initBiome(BiomeType type);
		};
	}
}