#pragma once

#include "component.h"

#include "private/external/gl.h"
#include "private/external/glm.h"

#include "renderer.h"

namespace TurnipRenderer {
	class DirectionalLight : public Component {
	public:
		DirectionalLight(glm::uvec2 shadowmapSize, glm::vec3 color)
			: shadowmapSize(shadowmapSize), color(color) {}
		
		const glm::uvec2 shadowmapSize;
	   	const glm::vec3 color;
	private:
		friend class DirectionalLightRenderer;
		bool initialized = false;
		ResourceHandle<const ColorBuffer> shadowmapColorBuffer = 0;
		ResourceHandle<const DepthBuffer> shadowmapDepthBuffer = 0;
		ResourceHandle<const FrameBuffer> shadowmapFramebuffer = 0;
	};
};
