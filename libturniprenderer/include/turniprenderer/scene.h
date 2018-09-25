#pragma once

#include <list>
#include <vector>
#include <memory>
#include <string>

#include "context_aware.h"
#include "memory/heirarchy.h"

namespace TurnipRenderer {
	class Entity;
	class SystemBase;

	// This is a bad comment to be writing, although it would be a funny example for the start of a talk
	// TODO: Thread Safety
	class Scene : ContextAware, Heirarchy<Entity> {
	public:
		Scene(Context& context);
		~Scene();
		using Heirarchy<Entity>::NodeBase;
		using Heirarchy<Entity>::heirarchy;

		void addModel(std::string path);
		
		template<typename... Args>
		Entity* addObjectToEndOfRoot(Args... args){
			return addObjectToIndexOfObject(*Heirarchy<Entity>::root, -1, args...);
		}
		template<typename... Args>
		Entity* addObjectToEndOfObject(Entity& newParent, Args... args){
			return addObjectToIndexOfObject(newParent, -1, args...);
		}
		template<typename... Args>
		Entity* addObjectToIndexOfObject(Entity& newParent, int relSiblingIndex, Args... args){
			auto* entity = (Entity*)Heirarchy<Entity>::addNode(
				std::make_unique<Entity>(context, *this, args...),
				&newParent, relSiblingIndex
				);
			return entity;
		}
		void reparentObject(Entity& sceneObject, Entity& newParent, int relSiblingIndex = -1){
			Heirarchy<Entity>::reparentNode(sceneObject, newParent, relSiblingIndex);
		}
		void unparentObject(Entity& sceneObject, int relSiblingIndex = -1){
			Heirarchy<Entity>::unparentNode(sceneObject, relSiblingIndex);
		}

		Entity* camera = nullptr;

		std::vector<std::unique_ptr<SystemBase>> systems;

		// TODO: Entity Deletion
		
	};
}
#include "entity.h"
#include "memory/heirarchy.impl"
