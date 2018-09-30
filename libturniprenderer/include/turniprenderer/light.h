#pragma once

#include "component.h"

#include "private/external/gl.h"
#include "private/external/glm.h"

namespace TurnipRenderer {
	class DirectionalLight : public Component {
	public:
		DirectionalLight(size_t sw, size_t sh, glm::vec3 color)
			: shadowmapWidth(sw), shadowmapHeight(sh), color(color) {}
		
		const size_t shadowmapWidth;
		const size_t shadowmapHeight;
		const glm::vec3 color;
	private:
		friend class DirectionalLightRenderer;
		bool initialized = false;
		GLuint shadowmapColorBuffer = 0;
		GLuint shadowmapDepthBuffer = 0;
		GLuint shadowmapDepthOnlyFramebuffer = 0;
		GLuint shadowmapFramebuffer = 0;
	};
};
