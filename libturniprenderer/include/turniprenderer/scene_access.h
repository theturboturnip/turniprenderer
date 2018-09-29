#pragma once

#include "component.h"
#include "context_scene_aware.h"

namespace TurnipRenderer {
	class SceneAccessComponent : protected Component, protected ContextSceneAware {
	public:
		SceneAccessComponent(Context& context, Scene& scene)
			: ContextSceneAware(context, scene) {}

		inline const Context& getContext() const {
			return context;
		}
		inline Context& getContext() {
			return context;
		}
		inline const Scene& getScene() const {
			return scene;
		}
		inline Scene& getScene() {
			return scene;
		}
	};
};
