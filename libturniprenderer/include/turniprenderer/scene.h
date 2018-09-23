#pragma once

#include <list>
#include <vector>
#include <memory>

#include "context_aware.h"
#include "memory/heirarchy.h"

namespace TurnipRenderer {
	class Entity;

	// This is a bad comment to be writing, although it would be a funny example for the start of a talk
	// TODO: Thread Safety
	class Scene : ContextAware, Heirarchy<Entity> {
	public:
		Scene(Context& context);
		using Heirarchy<Entity>::NodeData;
		using Heirarchy<Entity>::heirarchy;
		
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
			return (Entity*)Heirarchy<Entity>::addNode(
				std::make_unique<Entity>(context, *this, args...),
				&newParent, relSiblingIndex
				);
		}
		void reparentObject(Entity& sceneObject, Entity& newParent, int relSiblingIndex = -1){
			Heirarchy<Entity>::reparentNode(sceneObject, newParent, relSiblingIndex);
		}
		void unparentObject(Entity& sceneObject, int relSiblingIndex = -1){
			Heirarchy<Entity>::unparentNode(sceneObject, relSiblingIndex);
		}
		//void queueObjectForDeleting(SceneObject& sceneObject);

		Entity* camera = nullptr;
	private:
		//void clearDeletionQueue();
				
		// The vector of objects that will be deleted on clearDeletionQueue.
		// Needs to be sortable and needs to not contain duplicates, but std::set and std::deque don't work for these requirements so std::vector is used
		//std::vector<SceneObjectHierarchyIter> sceneObjectsQueuedForDeletion;
	};
}
#include "scene_object.h"
#include "memory/heirarchy.impl"
//#include "scene.impl"
/*#include "scene_object.h"
// TODO: Relocate to scene.impl
void Scene::queueObjectForDeleting(SceneObject* sceneObject){
	// TODO: Set flag on sceneObject?
	sceneObjectsQueuedForDeletion.push_back(sceneObject->heirarchyData.iter);
	for (auto childObject : sceneObject->heirarchyData.children)
		queueObjectForDeleting(childObject);
}

void Scene::clearDeletionQueue(){
	auto eraseObjectFromUniquePtrVector = [this](SceneObject* toErase){
		for (auto iter = sceneObjects.begin(); iter != sceneObjects.end(); iter++){
			if (*iter.get() = toErase){
				sceneObjects.erase(iter);
				return;
			}
		}
	};

	// Sort in reverse, so the items furthest down in the heirarchy are destroyed first.
	std::sort(sceneObjectsQueuedForDeletion.rbegin(), sceneObjectsQueuedForDeletion.rend());
	// Remove dupes from the queue
	sceneObjectsQueuedForDeletion.erase(std::unique(sceneObjectsQueuedForDeletion.begin(), sceneObjectsQueuedForDeletion.end()), sceneObjectsQueuedForDeletion.end());
			
	// This code relies on the contract that for any object in the deletion queue, all children objects are also in the deletion queue.
	// The parent object must also be destroyed last, which the reverse sort will ensure.
	for (auto iterToDelete : sceneObjectsQueuedForDeletion) {
		auto& siblingVec = (*iterToDelete)->heirarchyData.parent->heirarchyData.children;
		for (auto iter = siblingVec.begin() + sceneObject.heirarchyData.siblingIndex + 1; iter != siblingVec.end(); iter++){
			iter->heirarchyData.siblingIndex--;
		}
		childrenVec.erase(siblingVec.begin() + sceneObject->heirarchyData.siblingIndex);

		sceneObjectsSortedByHeirarchy.erase(iterToDelete);
		eraseObjectFromUniquePtrVector(*iterToDelete);
	}
	sceneObjectsQueuedForDeletion.clear();
}
*/
