#pragma once

#include "context_aware.h"

namespace TurnipRenderer {
	class Scene;

	class ContextSceneAware : protected ContextAware {
	protected:
		ContextSceneAware(Context& context, Scene& scene)
			: ContextAware(context), scene(scene) {}
		Scene& scene;
	};
}
