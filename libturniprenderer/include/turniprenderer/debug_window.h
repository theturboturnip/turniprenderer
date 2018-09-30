#pragma once

#include "context_aware.h"

namespace TurnipRenderer {
	class Entity;
}
namespace TurnipRenderer::Debug {	
	class DebugWindow : ContextAware {
	public:
		DebugWindow(Context& context) : ContextAware(context) {}

		void show();

	private:
		Entity* selected = nullptr;
	};
};
