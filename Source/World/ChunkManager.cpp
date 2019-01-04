#include "ChunkManager.h"
#include "Math/Vectors/Vec2.h"
#include <future>
#include "Utils/Clock.h"

namespace Minecraft {
	namespace World {

		using namespace Engine;
		using namespace Math;

		ChunkManager* ChunkManager::manager = nullptr;
		const int ChunkManager::WORLD_SIZE = 6000;

		const int ChunkManager::HEIGHT_NOISE_OCTAVE_COUNT = 2;
		const double ChunkManager::HEIGHT_NOISE_FREQUENCY = 1.0;
		const double ChunkManager::HEIGHT_NOISE_PERSISTENCE = 2.0;
		const double ChunkManager::HEIGHT_MAP_BOUNDS = 50.0;

		const int ChunkManager::BIOME_NOISE_OCTAVE_COUNT = 1;
		const double ChunkManager::BIOME_NOISE_FREQUENCY = 0.4;
		const double ChunkManager::BIOME_NOISE_PERSISTENCE = 2.0;
		const double ChunkManager::BIOME_NOISE_BOUNDS = 50.0;
		


		ChunkManager::ChunkManager() : cubeData(cubeVerticesUV, sizeof(cubeVerticesUV), GL_STATIC_DRAW), shader(nullptr), worldLoaded(false) {
			NoiseModuleData heightData(HEIGHT_NOISE_OCTAVE_COUNT, HEIGHT_NOISE_FREQUENCY, HEIGHT_NOISE_PERSISTENCE);
			heightMap = new NoiseMap(IVec2(WORLD_SIZE), Vec2(HEIGHT_MAP_BOUNDS), heightData);
		}

		ChunkManager::~ChunkManager() {
			for (Chunk* chunk : chunks)
				delete chunk;
			delete heightMap;
			delete biomeMap;
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
			Utils::Clock c;
			unsigned int coreCount = std::thread::hardware_concurrency();
			unsigned int chunksPerWorker = 48 / coreCount;
			std::thread* workers = new std::thread[coreCount];
			IVec2 startPos(0, 0);
			IVec2 endPos(20, 3);
			for (unsigned int i = 0; i < coreCount; i++) {
				workers[i] = std::thread(&ChunkManager::addChunkRange, this, startPos, endPos);
				startPos.y += 3;
				endPos.y += 3;
			}
			for (unsigned int i = 0; i < coreCount; i++) {
				workers[i].join();
			}
			std::cout << "Time : " << c.getTimePassed() << std::endl;
			std::cout << "Size : " << chunks.size() << std::endl;
		}

		void ChunkManager::initWorldGLData() {
			for (Chunk* chunk : chunks)
				chunk->initData(cubeData);
		}

		void ChunkManager::addChunkRange(const IVec2& startPosition, const IVec2& endPosition) {
			for (int i = startPosition.x; i < endPosition.x; i++) {
				for (int j = startPosition.y; j < endPosition.y; j++)
					addChunk(IVec2(i, j));
			}
			
		}


		void ChunkManager::addChunk(const IVec2& chunkPosition) {
			static std::mutex lock;
			static std::atomic<bool> type = false;
			BiomeType b = type ? BiomeType::FOREST : BiomeType::DESERT;
			type = !type;
			Chunk* chunk = new Chunk(*heightMap, chunkPosition, b);
			std::lock_guard<std::mutex> g(lock);
			chunks.push_back(chunk);
		} 

		bool isInTriangle(const Vec3& A, const Vec3& B, const Vec3& C, const Vec3& point) {
			float s = A.z * C.x - A.x * C.z + (C.z - A.z) * point.x + (A.x - C.x) * point.z;
			float t = A.x * B.z - A.z * B.x + (A.z - B.z) * point.x + (B.x - A.x) * point.z;

			if ((s < 0) != (t < 0))
				return false;

			float d = -B.z * C.x + A.z * (C.x - B.x) + A.x * (B.z - C.z) + B.x * C.z;

			return d < 0 ?
				(s <= 0 && s + t >= d) :
				(s >= 0 && s + t <= d);
		}

		void ChunkManager::updateRenderList(const Graphics::Camera& camera) {
			renderedChunks.clear();
			const Vec3& playerPosition = camera.getPosition();
			Vec2 player2DPosition(playerPosition.x, playerPosition.z);
			std::vector<Chunk*> cullByDistance;
			for (Chunk* chunk : chunks) {
				float distance = player2DPosition.distance(chunk->getCenter());
				if (distance < 40)
					renderedChunks.push_back(chunk);
				else if (distance < 150)
					cullByDistance.push_back(chunk);
			}
			auto edges = getFovEdges(camera);
			std::vector<Chunk*> cullByVisibilty;
			for (Chunk* chunk : cullByDistance) {
				if (isInFov(playerPosition, edges.first, edges.second, chunk->getAABB2D()))
					cullByVisibilty.push_back(chunk);
			}
			std::copy(cullByVisibilty.begin(), cullByVisibilty.end(), std::back_inserter(renderedChunks));
		}

		void ChunkManager::render() {
			for (const Chunk* chunk : renderedChunks)
				chunk->render(*shader);
		}

		std::pair<Vec3, Vec3> ChunkManager::getFovEdges(const Graphics::Camera& camera) const {
			Vec3 pointA = camera.getPosition();
			Vec3 direction = camera.getViewingDirection().normalized();
			// Cameras FOV is 45 degrees
			float angle = 60.0f;
			float s1 = std::sin(toRadians(angle)), s2 = std::sin(toRadians(-angle));
			float c1 = std::cos(toRadians(angle)), c2 = std::cos(toRadians(-angle));
			Vec3 posDirection(direction.x * c1 - direction.z * s1, 0, direction.z * c1 + direction.x * s1);
			Vec3 negDirection(direction.x * c2 - direction.z * s2, 0, direction.z * c2 + direction.x * s2);
			Vec3 pointB = pointA + posDirection * 500;
			Vec3 pointC = pointA + negDirection * 500;
			//std::cout << "A : " << pointA << "\nB : " << pointB << "\nC : " << pointC << std::endl;
			return std::pair<Vec3, Vec3>(pointB, pointC);
		}

		bool ChunkManager::isInFov(const Vec3& A, const Vec3& B, const Vec3& C, const AABB2D& boundingBox) const {
			const Vec2* data = (Vec2*) &boundingBox;
			for (int i = 0; i < 4; i++) {
				Vec3 point(data->x, 0, data->y);
				if (isInTriangle(A, B, C, point))
					return true;
			}
			return false;
		}

		BiomeType ChunkManager::getBiome(const Chunk* chunk) const {

		}



	}
}