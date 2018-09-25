#pragma once

#include "turniprenderer/system.h"
#include "component.h"

class FreeCameraSystem : public TurnipRenderer::System<TurnipRenderer::ComponentSet<FreeCameraComponent>, TurnipRenderer::ComponentSet<FreeCameraComponent>> {
public:
	FreeCameraSystem(TurnipRenderer::Context& context) : System(context) {}
	void runOnEntity(TurnipRenderer::Entity* entity) override;
};
