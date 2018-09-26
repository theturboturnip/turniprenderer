#pragma once

namespace TurnipRenderer {
	class Component{
	public:
		virtual ~Component() = default;
	};
	template<class ComponentType>
	struct is_component_type : std::integral_constant<bool,
													  std::is_class<ComponentType>::value &&
													  std::is_base_of<Component, ComponentType>::value>{};
}
