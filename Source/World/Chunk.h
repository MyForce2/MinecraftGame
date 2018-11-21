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
#include "HeightMap.h"
#include <unordered_map>

namespace Minecraft {
	namespace World {

		struct BlockData {
			Math::Vec3 textures;
			bool visible;
		};

		class Chunk {
		private:
			std::unordered_map<Math::Vec3, BlockData> blocksData;
			std::vector<Math::Mat4> renderData;
			std::vector<Math::Vec3> cubesTextures;
			const HeightMap& map;
			Math::IVec2 chunkCoordinate;
			Math::Vec2 center;

			Graphics::VertexArray vao;
			Graphics::VertexBuffer* matrixBuffer;
			Graphics::VertexBuffer* textureDataBuffer;
			const Graphics::VertexBuffer& CUBE_DATA;


			static const int CHUNK_MAX_HEIGHT;
			static const int CHUNK_SIZE;
			static const int SURFACE_MAX_HEIGHT;
			static const int CHUNK_TOP_CUBES;
			static const int DOWNWARDS_HEIGHT;

		public:
			Chunk(const HeightMap& map, const Math::IVec2& chunkCoordinate, const Graphics::VertexBuffer& vertices);
			~Chunk();

			void render(const Graphics::Shader& shader) const;
			inline const Math::IVec2& getChunkPosition() const { return chunkCoordinate; }
			inline const Math::Vec2& getCenter() const { return center; }

		private:
			void initBlocks();
			void initData();
			int getHeight(float heightValue) const;
			Math::Vec3 getTextureData(int height, int y) const;
		};
	}
}