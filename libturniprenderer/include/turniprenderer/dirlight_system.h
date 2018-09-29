#pragma once

#include "light.h"
#include "system.h"
#include "scene_access.h"

namespace TurnipRenderer {
	class DirectionalLightRenderer : public System<InputComponents<SceneAccessComponent, DirectionalLight, Transform>, OutputComponents<DirectionalLight>> {
	public:
		using System::System;
	protected:
		void runOnEntity(Entity*, const System::Inputs, const System::Outputs) override;
	};
};
