#include "Chunk.h"

#include "Graphics/Rendering/BasicRenderer.h"

namespace Minecraft {
	namespace World {

		using namespace Math;
		using namespace Graphics;

		const int Chunk::CHUNK_MAX_HEIGHT = 128;
		const int Chunk::CHUNK_SIZE = 20;
		const int Chunk::SURFACE_MAX_HEIGHT = 40;
		const int Chunk::DOWNWARDS_HEIGHT = CHUNK_MAX_HEIGHT - SURFACE_MAX_HEIGHT;
		//const VertexBuffer Chunk::CUBE_DATA = VertexBuffer(cubeVerticesUV, sizeof(cubeVerticesUV), GL_STATIC_DRAW);

		Chunk::Chunk(const utils::NoiseMap& map, const IVec2& chunkCoordinate, Graphics::VertexBuffer* d) :
			map(map), chunkCoordinate(chunkCoordinate), CUBE_DATA(d) {
			initBlocks();
		}

		Chunk::~Chunk() {
			delete matrixBuffer;
			delete textureDataBuffer;
		}

		void Chunk::render(const Shader& shader) const {
			BasicRenderer r;
			r.renderArraysInstanced(vao, shader, 0, 36, renderData.size());
		}

		void Chunk::initBlocks() {
			auto getHeight = [](float heightValue) -> int {
				heightValue += 1;
				heightValue /= 2;
				int height = heightValue * SURFACE_MAX_HEIGHT;
				return height <= 0 ? 1 : height;
			};

			auto getTextureData = [](int height, int y) -> Vec3 {
				if (height - y == 1)
					return Vec3(1.0f, 2.0f, 0.0f);
				if (height - y < 11)
					return Vec3(2.0f);
				return Vec3(3.0f);
			};

			for (int x = 0; x < CHUNK_SIZE; x++) {
				int height = getHeight(map.GetValue(x, 0)) + DOWNWARDS_HEIGHT + 1;
				int heightTwo = getHeight(map.GetValue(x, CHUNK_SIZE - 1)) + DOWNWARDS_HEIGHT + 1;
				for (int y = 0; y < height; y++) {
					Vec3 textureData = getTextureData(height, y);
					blocks.push_back(Block(Vec3(x, y, 0), textureData));
					renderData.push_back(Mat4::translation(Vec3(x, y, 0)));
					texturePositions.push_back(textureData);
				}
				for (int y = 0; y < heightTwo; y++) {
					Vec3 textureData = getTextureData(height, y);
					blocks.push_back(Block(Vec3(x, y, CHUNK_SIZE - 1), textureData));
					renderData.push_back(Mat4::translation(Vec3(x, y, 0)));
					texturePositions.push_back(textureData);
				}
			}

			for (int z = 0; z < CHUNK_SIZE; z++) {
				int height = getHeight(map.GetValue(0, z)) + DOWNWARDS_HEIGHT + 1;
				int heightTwo = getHeight(map.GetValue(0, CHUNK_SIZE - 1)) + DOWNWARDS_HEIGHT + 1;
				for (int y = 0; y < height; y++) {
					Vec3 textureData = getTextureData(height, y);
					blocks.push_back(Block(Vec3(0, y, z), textureData));
					renderData.push_back(Mat4::translation(Vec3(0, y, z)));
					texturePositions.push_back(textureData);
				}
				for (int y = 0; y < heightTwo; y++) {
					Vec3 textureData = getTextureData(height, y);
					blocks.push_back(Block(Vec3(0, y, CHUNK_SIZE - 1), textureData));
					renderData.push_back(Mat4::translation(Vec3(0, y, CHUNK_SIZE - 1)));
					texturePositions.push_back(textureData);
				}
			}

			for (int x = 0; x < CHUNK_SIZE; x++) {
				for (int z = 0; z < CHUNK_SIZE; z++) {
					int height = getHeight(map.GetValue(x, z)) + DOWNWARDS_HEIGHT + 1;
					for (int y = 0; y < height; y++) {
						Vec3 textureData = getTextureData(height, y);
						blocks.push_back(Block(Vec3(x, y, z), textureData));
					}
					renderData.push_back(Mat4::translation(Vec3(x, height, z)));
					renderData.push_back(Mat4::translation(Vec3(x, 0, z)));
					texturePositions.push_back(Vec3(1.0f, 2.0f, 0.0f));
					texturePositions.push_back(Vec3(3.0f));
				}
			}
		}

		void Chunk::initData() {
			matrixBuffer = new VertexBuffer(renderData.data(), sizeof(Mat4) * renderData.size(), GL_DYNAMIC_DRAW);
			textureDataBuffer = new VertexBuffer(texturePositions.data(), sizeof(Vec3) * texturePositions.size(), GL_DYNAMIC_DRAW);
			VBLayout layout;
			layout.addElement(3, GL_FLOAT);
			layout.addElement(3, GL_FLOAT);
			layout.addElement(2, GL_FLOAT);
			VBLayout textureLayout;
			textureLayout.addElement(3, GL_FLOAT);
			vao.addBuffer(*CUBE_DATA, layout);
			vao.addInstancedBuffer(*textureDataBuffer, textureLayout);
			vao.addInstancedMatrixBuffer(*matrixBuffer);
		}
	}
}