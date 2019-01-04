#pragma once

#include <vector>

#include <noise/noise.h>
#include <noiseutils.h>

#include "Block.h"
#include "Math/Vectors/IVec2.h"
#include "Graphics/Buffers/VertexBuffer.h"
#include "Graphics/VertexArray.h"
#include "Graphics/Shader.h"
#include "CubeData.h"
#include "NoiseMap.h"
#include <map>
#include <unordered_map>
#include <memory>
#include <atomic>
#include "Biomes/Biome.h"
#include "AABB2D.h"

namespace Minecraft {
	namespace World {

		struct BlockData {
			// Which 3 textures does this cube use
			Math::Vec3 textures;
			// Is this cube currently visible
			bool visible;
		};


		class Chunk {
		private:

			// Actual chunk data

			// Maps cube data to a position in the world
			std::unordered_map<Math::Vec3, BlockData> blocksData;
			// Model matrices for all of the cubes
			std::vector<Math::Mat4> renderData;
			// 3 Textures each cube uses
			std::vector<Math::Vec3> cubesTextures;
			// A heightmap for the world
			NoiseMap& map;
			// This chunk coordinate in the world
			Math::IVec2 chunkCoordinate;
			// The center position of this chunk in the world
			Math::Vec2 center;
			// This chunk 2D AABB
			AABB2D boundingSquare;
			// This chunks biome
			Biome biome;

			// OpenGL only chunk data

			Graphics::VertexArray* vao;
			Graphics::VertexBuffer* matrixBuffer;
			Graphics::VertexBuffer* textureDataBuffer;
			const Graphics::VertexBuffer* CUBE_DATA;

			friend class ChunkManager;

			static const std::atomic<int> CHUNK_MAX_HEIGHT;
			static const std::atomic<int> CHUNK_SIZE;
			static const std::atomic<int> SURFACE_MAX_HEIGHT;
			static const std::atomic<int> CHUNK_TOP_CUBES;
			static const std::atomic<int> DOWNWARDS_HEIGHT;

		public:
			Chunk(NoiseMap& map, const Math::IVec2& chunkCoordinate, BiomeType type);
			~Chunk();

			void render(const Graphics::Shader& shader) const;
			inline const Math::IVec2& getChunkPosition() const { return chunkCoordinate; }
			inline const Math::Vec2& getCenter() const { return center; }
			inline const AABB2D getAABB2D() const { return boundingSquare; }

		private:
			// Inits the chunk's blocks
			void initBlocks();
			// Inits the chunk's trees
			void initTrees();
			// Inits the chunks OpenGL data (Rendering data)
			void initData(const Graphics::VertexBuffer& vbo);

			// Returns the world space height for a normalized height value
			int getHeight(float heightValue) const;
			// Gets the texture data for a block according to its height compared to the max column height
			Math::Vec3 getTextureData(int height, int y) const;
			// Returns the height of a certain column in this chunk
			int getColumnHeight(float x, float z) const;
		};
	}
}