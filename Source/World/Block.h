#pragma once

#include "Math/Mat4.h"
#include "Math/Vectors/Vec3.h"

namespace Minecraft {
	namespace World	{

		using namespace Engine;

		class Block {
		private:
			Math::Vec3 position;
			Math::Vec3 textureData;


		public:
			Block(const Math::Vec3& position, const Math::Vec3& textureData);

			inline const Math::Vec3& getPosition() const { return position; }
			inline const Math::Vec3& getTextureData() const { return textureData; }
		};
	}
}
