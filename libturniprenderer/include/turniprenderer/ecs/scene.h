#pragma once

#include <list>
#include <vector>
#include <memory>
#include <string>

#include "context_aware.h"
#include "data/hierarchy.h"

#include "impl/scene_hierarchy_base.h"
#include "entity.h"

namespace TurnipRenderer {
	class SystemBase;
	// This is a bad comment to be writing, although it would be a funny example for the start of a talk
	// TODO: Thread Safety
	class Scene : ContextAware, Impl::SceneHierarchyBase {
	public:
		Scene(Context& context);
		~Scene();

		std::list<Entity*>& heirarchy;

		void addModel(std::string path);

		// TODO: Perfect Forwarding
		template<typename... Args>
		Entity& addObjectToEndOfRoot(Args... args){
			return addObjectToIndexOfObject(Impl::SceneHierarchyBase::root, -1, args...);
		}
		template<typename... Args>
		Entity& addObjectToEndOfObject(Entity& newParent, Args... args){
			return addObjectToIndexOfObject(newParent, -1, args...);
		}
		template<typename... Args>
		Entity& addObjectToIndexOfObject(Entity& newParent, int relSiblingIndex, Args... args){
			auto& entity = Impl::SceneHierarchyBase::addNode(
				newParent, relSiblingIndex,
				context, *this, args...
				);
			return entity;
		}
		void reparentObject(Entity& sceneObject, Entity& newParent, int relSiblingIndex = -1){
			Impl::SceneHierarchyBase::reparentNode(sceneObject, newParent, relSiblingIndex);
		}
		void unparentObject(Entity& sceneObject, int relSiblingIndex = -1){
			Impl::SceneHierarchyBase::unparentNode(sceneObject, relSiblingIndex);
		}

		Entity* camera = nullptr;

		std::vector<std::unique_ptr<SystemBase>> systems;

		// TODO: Entity Deletion
		
	};
}
