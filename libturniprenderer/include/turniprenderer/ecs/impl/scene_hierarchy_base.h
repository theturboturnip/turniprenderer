#pragma once

#include "entity_content.h"
#include "data/hierarchy.h"

namespace TurnipRenderer::Impl {
	using SceneHierarchyBase = Hierarchy<Impl::EntityContent>;
}
