#pragma once

#include "turniprenderer/external/glm.h"

#include "turniprenderer/resource_manager.h"
#include "turniprenderer/rendering/buffers.h"

namespace TurnipRenderer {
	struct Shadowmap {
		ResourceHandle<const ColorBuffer> colorBuffer;
		ResourceHandle<const DepthBuffer> depthBuffer;
		glm::mat4 VP;
		glm::mat4 V;
	};
}
