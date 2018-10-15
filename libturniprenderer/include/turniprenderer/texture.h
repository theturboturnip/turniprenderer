#pragma once

#include "private/external/gl.h"

#include <vector>

namespace TurnipRenderer {
	class Texture {
	public:
		Texture(std::vector<unsigned char>&& assetData);
		GLuint textureId = 0;
		GLuint64 bindlessTextureId = 0;
		int width;
		int height;
		int channels;
		std::vector<unsigned char> data;
	};
};
