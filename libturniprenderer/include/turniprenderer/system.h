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
		template<class SelectedContainerType>
		static inline bool fillContainer(Entity* entity, SelectedContainerType& container){
			return true;
		}
	};
	template<class T, class... TRemaining>
	class ComponentSet<T, TRemaining...> : ComponentSet<TRemaining...> {
	public:
		static_assert(!std::is_pointer<T>::value, "ComponentSet cannot contain a pointer type!");
		static_assert(!std::is_reference<T>::value, "ComponentSet cannot contain a reference type!");
		static_assert(std::is_base_of<Component, T>::value, "ComponentSet contains a type that isn't a component!");
		
		/*static bool ownsAComponent(Entity* entity){
			for (const auto& component : entity->components){
				if (dynamic_cast<T*>(component.get()) != nullptr)
					return true;
			}
			return false;
		}
		static inline bool entityHasAllComponents(Entity* entity){
			if (!ownsAComponent(entity)){
				return false;
			}
			return ComponentSet<TRemaining...>::entityHasAllComponents(entity);
		}*/

		/*template<template<class...> SrcType>
		static inline SrcType<T, TRemaining...> createContainer(){
			return SrcType<T, TRemaining...>();
			}*/
		// TODO: These could probably use the same thing as a base
		template<template<class...> class SrcType>
		using PointerContainerType = SrcType<
			typename std::add_pointer<typename std::remove_const<T>::type>::type,
			typename std::add_pointer<typename std::remove_const<TRemaining>::type>::type...
			>;
		template<template<class...> class SrcType>
		using ConstPointerContainerType = SrcType<
			typename std::add_pointer<typename std::add_const<T>::type>::type,
			typename std::add_pointer<typename std::add_const<TRemaining>::type>::type...
			>;
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
		// TODO: Find a way to do this without the bool Const
		template<class SelectedContainerType, bool Const>
		static inline bool fillContainer(Entity* entity, SelectedContainerType& container){
			for (const auto& component : entity->components){
				T* convertedPtr = dynamic_cast<T*>(component.get());
				if (convertedPtr != nullptr){
					std::get<
						typename std::add_pointer<typename apply_if<std::add_const, T, Const>::type>::type
						>(container) = convertedPtr;
					return ComponentSet<TRemaining...>::fillContainer(entity, container);
				}
			}
			return false;
		}
	};

	class SystemBase : protected ContextAware {
	public:
		SystemBase(Context& context) : ContextAware(context) {}
		
		virtual bool runOnEntityIfValid(Entity*) = 0;
		virtual ~SystemBase() = default;
	};
	template<class InputStorage, class OutputStorage, template<class...> class ComponentContainerType = std::tuple>
	class System : public SystemBase {
		using SystemBase::SystemBase;
		
		static_assert(std::is_base_of<ComponentSet<>, InputStorage>::value);
		static_assert(std::is_base_of<ComponentSet<>, OutputStorage>::value);
		static_assert(!std::is_same<ComponentSet<>, OutputStorage>::value, "A system must output something!");
	public:
		using Inputs = typename InputStorage::template ConstPointerContainerType<ComponentContainerType>;
		using Outputs = typename OutputStorage::template PointerContainerType<ComponentContainerType>;
		//		using Test = typename Inputs::A;
		//using Test2 = typename Outputs::A;

		bool runOnEntityIfValid(Entity* entity) override {
			Inputs inputs;
			Outputs outputs;
			if (!InputStorage::template fillContainer<Inputs, true>(entity, inputs)) return false;
			if (!OutputStorage::template fillContainer<Outputs, false>(entity, outputs)) return false;
			runOnEntity(entity, inputs, outputs);
			return true;
		}
	protected:
		virtual void runOnEntity(Entity*, const Inputs, const Outputs) = 0;
	};
};
