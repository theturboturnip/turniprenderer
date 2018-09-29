#pragma once

#include "component.h"

#include "private/external/gl.h"

namespace TurnipRenderer {
	class DirectionalLight : public Component {
	public:
		DirectionalLight(size_t sw, size_t sh)
			: shadowmapWidth(sw), shadowmapHeight(sh) {}
		
		const size_t shadowmapWidth;
		const size_t shadowmapHeight;
	private:
		friend class DirectionalLightRenderer;
		bool initialized = false;
		GLuint shadowmapColorBuffer = 0;
		GLuint shadowmapDepthBuffer = 0;
		GLuint shadowmapFramebuffer = 0;
	};
};
