#pragma once

#include "turniprenderer/external/gl.h"
#include "turniprenderer/util/context_aware.h"
#include "turniprenderer/rendering/buffers.h"

#include <vector>

namespace TurnipRenderer {
	class Texture : ContextAware {
	public:
		Texture(Context& context, std::vector<unsigned char>&& data, int width, int height, int channels);

		ResourceHandle<const ColorBuffer> buffer;
		
		std::vector<unsigned char> data;
	};
};
