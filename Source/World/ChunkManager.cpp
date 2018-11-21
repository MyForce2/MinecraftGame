#include "ChunkManager.h"
#include "Math/Vectors/Vec2.h"

namespace Minecraft {
	namespace World {

		using namespace Engine;
		using namespace Math;

		const int ChunkManager::NOISE_OCTAVE_COUNT = 1;
		const int ChunkManager::WORLD_SIZE = 2000;
		const double ChunkManager::NOISE_FREQUENCY = 0.4;
		const double ChunkManager::NOISE_PERSISTENCE = 2.0;
		const double ChunkManager::HEIGHT_MAP_BOUNDS = 50.0;
		ChunkManager* ChunkManager::manager = nullptr;

		ChunkManager::ChunkManager() : cubeData(cubeVerticesUV, sizeof(cubeVerticesUV), GL_STATIC_DRAW), shader(nullptr){
			NoiseModuleData data(NOISE_OCTAVE_COUNT, NOISE_FREQUENCY, NOISE_PERSISTENCE);
			heightMap = new HeightMap(IVec2(WORLD_SIZE), Vec2(HEIGHT_MAP_BOUNDS), data);
		}

		ChunkManager::~ChunkManager() {
			for (Chunk* chunk : chunks)
				delete chunk;
			delete heightMap;
		}

		ChunkManager* ChunkManager::getManager() {
			if (manager == nullptr)
				manager = new ChunkManager();
			return manager;
		}

		void ChunkManager::deleteManager() {
			if (manager != nullptr)
				delete manager;
		}

		bool ChunkManager::hasChunk(const IVec2& chunkPosition) const {
			for (const Chunk* chunk : chunks) {
				const IVec2& cPos = chunk->getChunkPosition();
				if (cPos.x == chunkPosition.x && cPos.y == chunkPosition.y)
					return true;
			}
			return false;
		}

		void ChunkManager::addChunk(const IVec2& chunkPosition) {
			int worldX = chunkPosition.x * 20;
			int worldY = chunkPosition.y * 20;
			if (worldX > WORLD_SIZE || worldY > WORLD_SIZE || worldX < 0 || worldY < 0)
				return;
			if (hasChunk(chunkPosition))
				return;
			Chunk* chunk = new Chunk(*heightMap, chunkPosition, cubeData);
			chunks.push_back(chunk);
		}

		void ChunkManager::updateRenderList(const Graphics::Camera& camera) {
			renderedChunks.clear();
			const Vec3& playerPosition = camera.getPosition();
			Vec2 player2DPosition(playerPosition.x, playerPosition.z);
			for (Chunk* chunk : chunks) {
				if (player2DPosition.distance(chunk->getCenter()) < 100)
					renderedChunks.push_back(chunk);
			}
			if (renderedChunks.empty())
				return;
			Vec4 viewDirection = Vec4(camera.getViewingDirection());
			Vec2 fovLimit1 = Vec2(Mat4::rotationZ(45) * viewDirection);
			Vec2 fovLimit2 = Vec2(Mat4::rotationZ(-45) * viewDirection);
			for (int i = 0; i < renderedChunks.size(); i++) {
				
			}
		}

		void ChunkManager::render() {
			for (const Chunk* chunk : renderedChunks)
				chunk->render(*shader);
		}

	}
}