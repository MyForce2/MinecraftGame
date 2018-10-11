#include "Block.h"


namespace Minecraft {
	namespace World {

		Block::Block(const Math::Vec3& position, const Math::Vec3& textureData) :
			position(position), textureData(textureData) {

		}
	}
}