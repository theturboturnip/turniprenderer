#pragma once

#include "private/external/gl.h"

namespace TurnipRenderer {
	class Texture {
	public:
		Texture(std::string contents){}
		GLuint textureId = 0;
	};
};
