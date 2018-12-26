#pragma once

#include "Chunk.h"
#include "HeightMap.h"
#include "Math/Vectors/IVec2.h"
#include "Graphics/Cameras/Camera.h"
#include "CubeData.h"

namespace Minecraft {
	namespace World {


		class ChunkManager {
		public:
			
			// Chunk manager game data
			
			std::vector<Chunk*> chunks;
			std::vector<Chunk*> renderedChunks;
			HeightMap* heightMap;
			Engine::Math::Vec3 playerPosition;
			bool worldLoaded;
			std::mutex chunksLock;
			
			// OpenGL graphics data

			Engine::Graphics::VertexBuffer cubeData;
			Engine::Graphics::Shader* shader;

			// Singleton instance

			static ChunkManager* manager;

			// Constants

			static const int NOISE_OCTAVE_COUNT;
			static const int WORLD_SIZE;
			static const double NOISE_FREQUENCY;
			static const double NOISE_PERSISTENCE;
			static const double HEIGHT_MAP_BOUNDS;
			static const char* VERTEX_SHADER_PATH;
			static const char* FRAGMENT_SHADER_PATH;

			
			
			ChunkManager();
			~ChunkManager();
	
		public:
			static ChunkManager* getManager();
			static void deleteManager();

			bool hasChunk(const Engine::Math::IVec2& chunkPosition) const;
			void addChunk(const Engine::Math::IVec2& chunkPosition);
			void addChunkRange(const Engine::Math::IVec2& startPosition, const Engine::Math::IVec2& endPosition);
			void loadWorld();
			void initWorldGLData();
			
			void updateRenderList(const Engine::Graphics::Camera& camera);
			void render();


			inline void setShader(Engine::Graphics::Shader* shader) { this->shader = shader; }
			inline const Engine::Graphics::Shader* getShader() const { return shader; }
		};
	}
}