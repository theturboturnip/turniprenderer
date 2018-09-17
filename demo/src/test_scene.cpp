#include "turniprenderer/context.h"
#include "turniprenderer/scene.h"
#include "turniprenderer/scene_object.h"

void testScene(){
	TurnipRenderer::Context context;
	TurnipRenderer::Scene scene(context);

	scene.addObject(glm::vec3(0));
}
