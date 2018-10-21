#pragma once

#include "engine_fwd.h"
#include "component.h"
#include "turniprenderer/util/context_aware.h"
#include "entity.h"

#include <type_traits>

namespace TurnipRenderer {
	template<bool Const, class... Ts>
	class ComponentSet;
	template<bool Const>
	class ComponentSet<Const> {
	public:
		template<class SelectedContainerType>
		static inline bool fillContainer(Entity* entity, SelectedContainerType& container){
			return true;
		}
	};
	template<bool Const, class T, class... TRemaining>
	class ComponentSet<Const, T, TRemaining...> : ComponentSet<Const, TRemaining...> {
	public:
		// TODO: Move this out
		template<template<class> class ToApply, class ApplyTo, bool If>
		struct apply_if;
		template<template<class> class ToApply, class ApplyTo>
		struct apply_if<ToApply, ApplyTo, true>{
			using type = typename ToApply<ApplyTo>::type;
		};
		template<template<class> class ToApply, class ApplyTo>
		struct apply_if<ToApply, ApplyTo, false>{
			using type = ApplyTo;
		};
		
		using ComponentType = typename std::remove_const<T>::type;
		template<class Type>
		using PointerType = typename std::add_pointer<typename apply_if<std::add_const, Type, Const>::type>::type;
		static_assert(is_component_type<ComponentType>::value);
		
		template<template<class...> class SrcType>
		using PointerContainerType = SrcType<
			PointerType<ComponentType>,
			PointerType<typename std::remove_const<TRemaining>::type>...
			>;
		template<class SelectedContainerType>
		static inline bool fillContainer(Entity* entity, SelectedContainerType& container){
			PointerType<ComponentType> componentPtr = entity->getComponent<T>();
			if (componentPtr == nullptr)
				return false;
			std::get<PointerType<ComponentType>>(container) = componentPtr;
			return ComponentSet<Const, TRemaining...>::fillContainer(entity, container);
		}
	};
	template<class... ComponentTypes>
	class InputComponents : public ComponentSet<true, ComponentTypes...>{};
	template<class... ComponentTypes>
	class OutputComponents : public ComponentSet<false, ComponentTypes...>{};

	class SystemBase : protected ContextAware {
	public:
		SystemBase(Context& context) : ContextAware(context) {}
		
		virtual bool runOnEntityIfValid(Entity*) = 0;
		virtual ~SystemBase() = default;
	};
	template<class InputStorage, class OutputStorage, template<class...> class ComponentContainerType = std::tuple>
	class System : public SystemBase {
		using SystemBase::SystemBase;
		
		static_assert(std::is_base_of<ComponentSet<true>, InputStorage>::value);
		static_assert(std::is_base_of<ComponentSet<false>, OutputStorage>::value);
		static_assert(!std::is_same<ComponentSet<false>, OutputStorage>::value, "A system must output something!");
	public:
		using Inputs = typename InputStorage::template PointerContainerType<ComponentContainerType>;
		using Outputs = typename OutputStorage::template PointerContainerType<ComponentContainerType>;
		
		bool runOnEntityIfValid(Entity* entity) override {
			Inputs inputs;
			Outputs outputs;
			if (!InputStorage::fillContainer(entity, inputs)) return false;
			if (!OutputStorage::fillContainer(entity, outputs)) return false;
			runOnEntity(entity, inputs, outputs);
			return true;
		}
	protected:
		template<class ComponentPtrType, class StorageType>
		inline ComponentPtrType getComponent(const StorageType& storage){
			static_assert(is_component_type<typename std::remove_pointer<ComponentPtrType>::type>::value);
			return std::get<ComponentPtrType>(storage);
		}
		
		virtual void runOnEntity(Entity*, const Inputs, const Outputs) = 0;
	};
};
