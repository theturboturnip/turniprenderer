#pragma once

#include "engine_fwd.h"
#include "component.h"
#include "context_aware.h"
#include "entity.h"

#include <type_traits>

namespace TurnipRenderer {
	// TODO: Figure out how to resolve parent-child dependencies. An object which depends on a parent property (Transforms come to mind) needs to run after that parent.
	// A soln. could be to use hierarchy depth, and to only multithread things on the same depth
	// However it would be nice to be able to determine whether 2 objects are "related" and use that
	
	// Jobs with no special access run first (multithreaded),
	// Jobs with transform access (input or output) run next (hybrid single/multithreaded depending on the heirarchy
	// jobs which modify the heirarchy run second (single-threaded),
	// jobs which render things run third (single-threaded)
	enum class JobLevel {
		Base = 0,
			Transform = 1,
			Hierarchy = 2,
			Renderer = 3
	};
	class JobBase {
	public:
		JobBase(JobLevel level, const Entity* target)
			: level(level), target(target) {}
		
		const JobLevel level = JobLevel::Base;
		const Entity* const target = nullptr;
		virtual void execute() const = 0;
		virtual ~JobBase() = default;
	};
	
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
	private:
		using ComponentType = typename std::remove_const<T>::type;
		template<class Type>
		using PointerType = typename std::add_pointer<typename apply_if<std::add_const, Type, Const>::type>::type;
		static_assert(is_component_type<ComponentType>::value);
		
	public:
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
		
		template<class TypeToFind>
		static inline bool containsType(){
			if (std::is_same<ComponentType, TypeToFind>::value) return true;
			return ComponentSet<Const, TRemaining...>::containsType<TypeToFind>();
		}
	};
	template<class... ComponentTypes>
	class InputComponents : public ComponentSet<true, ComponentTypes...>{};
	template<class... ComponentTypes>
	class OutputComponents : public ComponentSet<false, ComponentTypes...>{};
	
	class SystemBase : protected ContextAware {
	public:
		const JobLevel level;
		SystemBase(Context& context, JobLevel level)
			: ContextAware(context), level(level) {}
		
		virtual std::unique_ptr<JobBase> createJob(Entity*) override = 0;
		virtual ~SystemBase() = default;
	};
	template<JobLevel Level, class InputStorage, class OutputStorage, template<class...> class ComponentContainerType = std::tuple>
	class System : public SystemBase {
		System(Context& context) : SystemBase(context, Level) {}
		
		static_assert(std::is_base_of<ComponentSet<true>, InputStorage>::value);
		static_assert(std::is_base_of<ComponentSet<false>, OutputStorage>::value);
		static_assert(!std::is_same<ComponentSet<false>, OutputStorage>::value, "A system must output something!");
	public:
		using Inputs = typename InputStorage::template PointerContainerType<ComponentContainerType>;
		using Outputs = typename OutputStorage::template PointerContainerType<ComponentContainerType>;

		std::unique_ptr<JobBase> createJob(Entity* entity) override {
			Inputs inputs;
			Outputs outputs;
			if (!InputStorage::fillContainer(entity, inputs)) return nullptr;
			if (!OutputStorage::fillContainer(entity, outputs)) return nullptr;
			return std::make_unique<Job>(entity, *this, inputs, outputs);
		}
	protected:
		class Job : public JobBase {
			Job(const Entity* target, const System& system, const Inputs inputs, const Outputs outputs)
				: JobBase(Level, target), system(system), inputs(inputs), outputs(outputs) {}
			
			const System& system;
			const Inputs inputs;
			const Outputs outputs;
			void execute const override (){
				system.execute(inputs, outputs);
			}
		};
		
		template<class ComponentPtrType, class StorageType>
		inline ComponentPtrType getComponent(const StorageType& storage){
			static_assert(is_component_type<typename std::remove_pointer<ComponentPtrType>::type>::value);
			return std::get<ComponentPtrType>(storage);
		}
		
		virtual void execute(const Inputs, const Outputs) const = 0;
	};
};
