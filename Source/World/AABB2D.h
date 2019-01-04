#pragma once

#include "Math/Vectors/Vec2.h"

namespace Minecraft {
	namespace World {

		/*
			A 2d variant of the 3d Axis Aligned Bounding Box (AABB)
		*/
		struct AABB2D {
			Engine::Math::Vec2 topLeft;
			Engine::Math::Vec2 topRight;
			Engine::Math::Vec2 botLeft;
			Engine::Math::Vec2 botRight;
		};
	}
}