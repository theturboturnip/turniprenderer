#pragma once

#include "engine_fwd.h"
#include "context_aware.h"
#include "entity.h"

namespace TurnipRenderer {
	template<class... Ts>
	class ComponentSet;
	template<>
	class ComponentSet<> {
	public:
		static void addOwnedComponent(Component* component, size_t& count){}
		static bool entityHasAllComponents(Entity* entity){
			return true;
		}
	};
	template<class T, class... TRemaining>
	class ComponentSet<T, TRemaining...> : ComponentSet<TRemaining...> {
	public:
		static_assert(std::is_base_of<Component, T>::value, "ComponentSet contains a type that isn't a component!");
		
		static bool ownsAComponent(Entity* entity){
			for (const auto& component : entity->components){
				if (dynamic_cast<T*>(component.get()) != nullptr)
					return true;
			}
			return false;
		}
		static bool entityHasAllComponents(Entity* entity){
			if (!ownsAComponent(entity)){
				return false;
			}
			return ComponentSet<TRemaining...>::entityHasAllComponents(entity);
		}
	};

	class SystemBase : protected ContextAware {
	public:
		SystemBase(Context& context) : ContextAware(context) {}
		
		virtual bool runOnEntityIfValid(Entity*) = 0;
		virtual ~SystemBase() = default;
	};
	template<class InputStorage, class OutputStorage>
	class System : public SystemBase {
		using SystemBase::SystemBase;
		
		static_assert(std::is_base_of<ComponentSet<>, InputStorage>::value);
		static_assert(std::is_base_of<ComponentSet<>, OutputStorage>::value);
	public:
		bool runOnEntityIfValid(Entity* entity) override {
			if (!InputStorage::entityHasAllComponents(entity)) return false;
			if (!OutputStorage::entityHasAllComponents(entity)) return false;
			runOnEntity(entity);
			return true;
		}
	protected:
		virtual void runOnEntity(Entity*) = 0;
	};
};
