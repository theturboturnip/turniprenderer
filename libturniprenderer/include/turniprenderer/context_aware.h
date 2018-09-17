#pragma once

namespace TurnipRenderer {
	class Context;

	class ContextAware {
	protected:
		// Protected Constructor => A ContextAware may only be created through a subclass
		ContextAware(Context& context) : context(context) {}
		Context& context;
	};
};
