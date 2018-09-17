#pragma once

#include "context_aware.h"

namespace TurnipRenderer {
	class Scene;

	class ContextSceneAware : ContextAware {
	protected:
		ContextSceneAware(Context& context, Scene& scene)
			: ContextAware(context), scene(scene) {}
		Scene& scene;
	};
}
