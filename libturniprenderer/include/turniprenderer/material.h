#pragma once

#include "resource.h"
#include "texture.h"

namespace TurnipRenderer {
	class Material {
	public:
		ResourceHandle<Texture> texture;
	};
};
