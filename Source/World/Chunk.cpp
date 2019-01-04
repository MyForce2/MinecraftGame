#include "Chunk.h"

#include "Graphics/Rendering/BasicRenderer.h"
#include <unordered_map>
#include <random>

namespace Minecraft {
	namespace World {

		using namespace Math;
		using namespace Graphics;

		const std::atomic<int> Chunk::CHUNK_MAX_HEIGHT = 64;
		const std::atomic<int> Chunk::CHUNK_SIZE = 20;
		const std::atomic<int> Chunk::SURFACE_MAX_HEIGHT = 50;
		const std::atomic<int> Chunk::DOWNWARDS_HEIGHT = CHUNK_MAX_HEIGHT - SURFACE_MAX_HEIGHT;
		const std::atomic<int> Chunk::CHUNK_TOP_CUBES = 5;

		Chunk::Chunk(NoiseMap& map, const IVec2& chunkCoordinate, BiomeType type) :
			map(map), chunkCoordinate(chunkCoordinate),
			center(chunkCoordinate.x * CHUNK_SIZE + CHUNK_SIZE / 2, chunkCoordinate.y * CHUNK_SIZE + CHUNK_SIZE / 2),
			biome(type)
				{
			boundingSquare.topLeft = Vec2(center.x - CHUNK_SIZE / 2, center.y + CHUNK_SIZE / 2);
			boundingSquare.topRight = Vec2(center.x + CHUNK_SIZE / 2, center.y + CHUNK_SIZE / 2);
			boundingSquare.botLeft = Vec2(center.x - CHUNK_SIZE / 2, center.y - CHUNK_SIZE / 2);
			boundingSquare.botRight = Vec2(center.x + CHUNK_SIZE / 2, center.y - CHUNK_SIZE / 2);
			initBlocks();
			initTrees();
		}

		Chunk::~Chunk() {
			delete matrixBuffer;
			delete textureDataBuffer;
		}

		void Chunk::render(const Shader& shader) const {
			BasicRenderer r;
			CUBE_DATA->bind();
			shader.bind();
			r.renderArraysInstanced(*vao, shader, 0, 36, renderData.size());
		}

		void Chunk::initBlocks() {
			auto vecAbs = [](const IVec2& vec) -> IVec2 { return IVec2(std::abs(vec.x), std::abs(vec.y)); };
			int centerHeight = getHeight(map[center]) + DOWNWARDS_HEIGHT + 1;
			biome = centerHeight > 32 ? BiomeType::FOREST : BiomeType::DESERT;
			IVec2 absChunkPos = vecAbs(chunkCoordinate);
			bool hasEdges = absChunkPos.x == 300 || absChunkPos.y == 300;
			for (int x = 0; x < CHUNK_SIZE; x++) {
				for (int z = 0; z < CHUNK_SIZE; z++) {
					// World position of this current column
					IVec2 worldPos(chunkCoordinate.x * CHUNK_SIZE + x, chunkCoordinate.y * CHUNK_SIZE + z);
					// Noise functions only accept positive values, so negative coordinates are converted to positive coordinates
					IVec2 absWorldPos = vecAbs(worldPos);
					int height = getHeight(map[absWorldPos]) + DOWNWARDS_HEIGHT + 1;
					int capHeight = height + CHUNK_TOP_CUBES;
					for (int y = 0; y < capHeight; y++) {
						Vec3 textureData = getTextureData(capHeight, y);
						blocksData[Vec3(worldPos.x, y, worldPos.y)] = { textureData, false };
					}
					for (int y = height; y < capHeight; y++) {
						Vec3 position(worldPos.x, y, worldPos.y);
						renderData.push_back(Mat4::translation(position));
						cubesTextures.push_back(blocksData[position].textures);
						blocksData[position].visible = true;
					}
					Vec3 position(worldPos.x, 0, worldPos.y);
					renderData.push_back(Mat4::translation(position));
					cubesTextures.push_back(blocksData[position].textures);
					blocksData[position].visible = true;
				}
			}

			if (!hasEdges)
				return;
		
			for (int x = 0; x < CHUNK_SIZE; x++) {
				// World position of this column 
				IVec2 worldPos(chunkCoordinate.x * CHUNK_SIZE + x, chunkCoordinate.y * CHUNK_SIZE);
				// World position of the edge column
				IVec2 worldPosEdge(worldPos.x, worldPos.y + CHUNK_SIZE - 1);
				// Noise functions only accept positive values, so negative coordinates are converted to positive coordinates
				IVec2 absWorldPos = vecAbs(worldPos);
				IVec2 absWorldPosEdge = vecAbs(worldPosEdge);
				int height = getHeight(map[absWorldPos]) + DOWNWARDS_HEIGHT + 1;
				int heightTwo = getHeight(map[absWorldPosEdge]) + DOWNWARDS_HEIGHT + 1;
				for (int y = 1; y < height; y++) {
					Vec3 position(worldPos.x, y, worldPos.y);
					renderData.push_back(Mat4::translation(position));
					cubesTextures.push_back(blocksData[position].textures);
					blocksData[position].visible = true;
				}
				for (int y = 1; y < heightTwo; y++) {
					Vec3 position(worldPos.x, y, worldPosEdge.y);
					renderData.push_back(Mat4::translation(position));
					cubesTextures.push_back(blocksData[position].textures);
					blocksData[position].visible = true;
				}
			}

			for (int z = 1; z < CHUNK_SIZE - 1;z++) {
				// World position of this column 
				IVec2 worldPos(chunkCoordinate.x * CHUNK_SIZE, chunkCoordinate.y * CHUNK_SIZE + z);
				// World position of the edge column
				IVec2 worldPosEdge(worldPos.x + CHUNK_SIZE - 1, worldPos.y);
				// Noise functions only accept positive values, so negative coordinates are converted to positive coordinates
				IVec2 absWorldPos = vecAbs(worldPos);
				IVec2 absWorldPosEdge = vecAbs(worldPosEdge);
				int height = getHeight(map[absWorldPos]) + DOWNWARDS_HEIGHT + 1;
				int heightTwo = getHeight(map[absWorldPosEdge]) + DOWNWARDS_HEIGHT + 1;
				for (int y = 1; y < height; y++) {
					Vec3 position(worldPos.x, y, worldPos.y);
					renderData.push_back(Mat4::translation(position));
					cubesTextures.push_back(blocksData[position].textures);
					blocksData[position].visible = true;
				}
				for (int y = 1; y < heightTwo; y++) {
					Vec3 position(worldPosEdge.x, y, worldPos.y);
					renderData.push_back(Mat4::translation(position));
					cubesTextures.push_back(blocksData[position].textures);
					blocksData[position].visible = true;
				}
			}
		}

		void Chunk::initTrees() {
			if (!biome.hasTrees())
				return;
			std::random_device rd;
			std::mt19937 generator(rd());
			std::uniform_int_distribution distribution(0, 100);
			int val = distribution(generator);
			if (val > 40)
				return;
			std::uniform_int_distribution xRange(chunkCoordinate.x * CHUNK_SIZE, (chunkCoordinate.x + 1) * CHUNK_SIZE);
			std::uniform_int_distribution zRange(chunkCoordinate.y * CHUNK_SIZE, (chunkCoordinate.y + 1) * CHUNK_SIZE);
			int x = xRange(generator);
			int z = zRange(generator);
			int y = getColumnHeight(x, z);
			if (y == 0)
				return;
			Vec3 position(x, y + 1, z);
			Vec3 treeTexture = biome.getTreeTexture();
			for (int i = 0; i < 5; i++) {
				blocksData[position] = { treeTexture, true };
				renderData.push_back(Mat4::translation(position));
				cubesTextures.push_back(treeTexture);
				position.y++;
			}
			if (!biome.hasLeaves())
				return;
			Vec3 leafTexture = biome.getLeafTexture();
			Vec3 leafStartPosition(position);
			leafStartPosition.x -= 1;
			leafStartPosition.z -= 1;
			for (int y = 0; y < 3; y++) {
				float leafY = float(leafStartPosition.y + y);
				for (int x = 0; x < 3; x++) {
					float leafX = float(leafStartPosition.x + x);
					for (int z = 0; z < 3; z++) {
						Vec3 leafPos(leafX, leafY, leafStartPosition.z + z);
						blocksData[leafPos] = { leafTexture, true };
						renderData.push_back(Mat4::translation(leafPos));
						cubesTextures.push_back(leafTexture);
					}
				}
			}

		}

		void Chunk::initData(const Graphics::VertexBuffer& vbo) {
			CUBE_DATA = new VertexBuffer(vbo);
			matrixBuffer = new VertexBuffer(renderData.data(), sizeof(Mat4) * renderData.size(), GL_DYNAMIC_DRAW);
			textureDataBuffer = new VertexBuffer(cubesTextures.data(), sizeof(Vec3) * cubesTextures.size(), GL_DYNAMIC_DRAW);
			vao = new VertexArray();
			VBLayout layout;
			layout.addElement(3, GL_FLOAT);
			layout.addElement(3, GL_FLOAT);
			layout.addElement(2, GL_FLOAT);
			VBLayout textureLayout;
			textureLayout.addElement(3, GL_FLOAT);
			vao->addBuffer(*CUBE_DATA, layout);
			vao->addInstancedBuffer(*textureDataBuffer, textureLayout);
			vao->addInstancedMatrixBuffer(*matrixBuffer);
		}

		int Chunk::getHeight(float heightValue) const {
			heightValue += 1;
			heightValue /= 2;
			int height = heightValue * SURFACE_MAX_HEIGHT;
			return height <= 0 ? 1 : height;
		}

		Vec3 Chunk::getTextureData(int height, int y) const {
			// Is this the top cube
			if (height - y == 1)
				return biome.getTopBlockTexture();
			// Is this the middle cube
			if (height - y < 11)
				return biome.getMidBlockTexture();
			return Biome::getBottomBlockTexture();
		}

		int Chunk::getColumnHeight(float x, float z) const {
			float y = 0.0f;
			for (auto pair : blocksData) {
				const Vec3& blockPosition = pair.first;
				if (blockPosition.x == x && blockPosition.z == z) {
					if (blockPosition.y > y)
						y = blockPosition.y;
				}
			}
			return int(y);
		}
	}
}