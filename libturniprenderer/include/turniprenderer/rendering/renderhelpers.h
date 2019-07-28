#pragma once

#include "turniprenderer/external/glm.h"

#include "turniprenderer/entity.h"
#include "turniprenderer/rendering/shadowmap.h"
#include "turniprenderer/rendering/renderer.h"

namespace TurnipRenderer::RenderHelpers {
	struct StaticRenderData {
		glm::mat4 transformProjectionFromWorld;
		Shadowmap* shadowmap;
		glm::vec3 cameraPos;
	};
	void drawEntity(Context& context,
					Entity& entity,
					StaticRenderData& renderData);
}
