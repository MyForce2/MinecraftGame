#pragma once

#include "Chunk.h"
#include "NoiseMap.h"
#include "Math/Vectors/IVec2.h"
#include "Graphics/Cameras/Camera.h"
#include "CubeData.h"
#include <memory>
#include "AABB2D.h"

namespace Minecraft {
	namespace World {



		class ChunkManager {
		private:

			friend class Chunk;
			
			// Chunk manager game data
			
			std::vector<Chunk*> chunks;
			std::vector<Chunk*> renderedChunks;
			NoiseMap* heightMap;
			NoiseMap* biomeMap;
			Engine::Math::Vec3 playerPosition;
			bool worldLoaded;
			std::mutex chunksLock;
			
			// OpenGL graphics data

			Engine::Graphics::VertexBuffer cubeData;
			Engine::Graphics::Shader* shader;

			// Singleton instance

			static ChunkManager* manager;

			// Constants

			// The size of the world (In blocks)
			static const int WORLD_SIZE;

			// Height map constants

			static const int HEIGHT_NOISE_OCTAVE_COUNT;
			static const double HEIGHT_NOISE_FREQUENCY;
			static const double HEIGHT_NOISE_PERSISTENCE;
			static const double HEIGHT_MAP_BOUNDS;

			// Biome map constants

			static const int BIOME_NOISE_OCTAVE_COUNT;
			static const double BIOME_NOISE_FREQUENCY;
			static const double BIOME_NOISE_PERSISTENCE;
			static const double BIOME_NOISE_BOUNDS;
			
			
			ChunkManager();
			~ChunkManager();
	
		public:
			// Singleton methods

			static ChunkManager* getManager();
			static void deleteManager();

			bool hasChunk(const Engine::Math::IVec2& chunkPosition) const;
			void addChunk(const Engine::Math::IVec2& chunkPosition);
			void addChunkRange(const Engine::Math::IVec2& startPosition, const Engine::Math::IVec2& endPosition);
			void loadWorld();
			void initWorldGLData();
			void updateRenderList(const Engine::Graphics::Camera& camera);
			void render();

			// Setters

			inline void setShader(Engine::Graphics::Shader* shader) { this->shader = shader; }
			inline void setPlayerPosition(const Engine::Math::Vec3& playerPosition) { this->playerPosition = playerPosition; }

			inline const Engine::Graphics::Shader* getShader() const { return shader; }

		private:
			std::pair<Engine::Math::Vec3, Engine::Math::Vec3> getFovEdges(const Engine::Graphics::Camera& camera) const;
			bool isInFov(const Engine::Math::Vec3& A, const Engine::Math::Vec3& B, const Engine::Math::Vec3& C, const AABB2D& boundingBox) const;
			BiomeType getBiome(const Chunk* chunk) const;
		};
	}
}