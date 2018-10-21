#pragma once

#include "turniprenderer/dirlight_component.h"
#include "turniprenderer/system.h"
#include "turniprenderer/scene_access.h"

namespace TurnipRenderer {
	class DirectionalLightRenderer : public System<InputComponents<SceneAccessComponent, DirectionalLight, Transform>, OutputComponents<DirectionalLight>> {
	public:
		using System::System;
	protected:
		void runOnEntity(Entity*, const System::Inputs, const System::Outputs) override;
	};
};
