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

namespace Minecraft {
	namespace World {

		class Chunk {
		private:
			std::vector<Block> blocks;
			std::vector<Math::Mat4> renderData;
			std::vector<Math::Vec3> texturePositions;
			const utils::NoiseMap& map;
			Math::IVec2 chunkCoordinate;

			Graphics::VertexArray vao;
			Graphics::VertexBuffer* matrixBuffer;
			Graphics::VertexBuffer* textureDataBuffer;
			Graphics::VertexBuffer* CUBE_DATA;


			static const int CHUNK_MAX_HEIGHT;
			static const int CHUNK_SIZE;
			static const int SURFACE_MAX_HEIGHT;
			static const int DOWNWARDS_HEIGHT;

		public:
			Chunk(const utils::NoiseMap& map, const Math::IVec2& chunkCoordinate, Graphics::VertexBuffer* d);
			~Chunk();

			void render(const Graphics::Shader& shader) const;

		private:
			void initBlocks();
			void initData();
		};
	}
}