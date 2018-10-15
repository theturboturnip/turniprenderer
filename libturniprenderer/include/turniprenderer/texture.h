#pragma once

#include "turniprenderer/private/external/gl.h"
#include "turniprenderer/context_aware.h"
#include "turniprenderer/renderer.h"

#include <vector>

namespace TurnipRenderer {
	class Texture : ContextAware {
	public:
		Texture(Context& context, std::vector<unsigned char>&& data, int width, int height, int channels);
		//std::vector<unsigned char>&& assetData);

		ResourceHandle<const ColorBuffer> buffer;
		
		std::vector<unsigned char> data;
	};
};
