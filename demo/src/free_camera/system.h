#pragma once

#include "turniprenderer/system.h"
#include "component.h"

class FreeCameraSystem : public TurnipRenderer::System<TurnipRenderer::ComponentSet<FreeCameraComponent>, TurnipRenderer::ComponentSet<TurnipRenderer::Transform>> {
public:
	FreeCameraSystem(TurnipRenderer::Context& context) : System(context) {}
	void runOnEntity(TurnipRenderer::Entity* entity, const System::Inputs inputs, const System::Outputs outputs) override;
};
