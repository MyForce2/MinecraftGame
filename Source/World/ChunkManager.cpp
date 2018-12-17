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

		ChunkManager::ChunkManager() : cubeData(cubeVerticesUV, sizeof(cubeVerticesUV), GL_STATIC_DRAW), shader(nullptr), worldLoaded(false) {
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

		void ChunkManager::loadWorld() {
			//unsigned int coreCount = 1;// std::thread::hardware_concurrency();
			//unsigned int chunksPerWorker = 2 / coreCount;
			//std::thread* workers = new std::thread[coreCount];
			//for (unsigned int i = 0; i < coreCount; i++) {
			//	std::cout << "K" << std::endl;
			//	IVec2 startPos(i * chunksPerWorker, i * chunksPerWorker);
			//	IVec2 endPos(startPos.x + chunksPerWorker, startPos.x + chunksPerWorker);
			//	std::thread t(&ChunkManager::addChunkRange, this, startPos, endPos);
			//	t.join();
			//	//addChunkRange(startPos, endPos);
			//}

			std::thread c(&ChunkManager::addChunk, this, IVec2(0, 1));
			c.join();
			addChunk(IVec2(1, 0));
			std::thread d(&ChunkManager::addChunk, this, IVec2(1, 1));
			d.join();
			
			//addChunkRange(IVec2(0), IVec2(2));
			for (Chunk* chunk : chunks)
				chunk->initData(cubeData);
			//delete[] workers;
			std::cout << "Size : " << chunks.size() << std::endl;
		}

		void ChunkManager::addChunkRange(const IVec2& startPosition, const IVec2& endPosition) {
			for (int i = startPosition.x; i < endPosition.x; i++) {
				for (int j = startPosition.y; j < endPosition.y; j++)
					addChunk(IVec2(i, j));
			}
		}

		void ChunkManager::addChunk(const IVec2& chunkPosition) {
			int worldX = chunkPosition.x * 20;
			int worldY = chunkPosition.y * 20;
			//if (worldX > WORLD_SIZE || worldY > WORLD_SIZE || worldX < 0 || worldY < 0)
			//	return;
			//if (hasChunk(chunkPosition))
			//	return;
			Chunk* chunk = new Chunk(*heightMap, chunkPosition);
			std::cout << "Chunk : " << Vec2(chunkPosition) << " Address : " << chunk << std::endl;
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
		}

		void ChunkManager::render() {
			for (const Chunk* chunk : renderedChunks)
				chunk->render(*shader);
		}

	}
}