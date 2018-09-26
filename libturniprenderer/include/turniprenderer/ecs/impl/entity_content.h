#pragma once

#include "ecs/component.h"
#include "context_scene_aware.h"
#include "private/external/glm.h"

#include "mesh.h"
#include "resource.h"
#include "transform.h"

#include <typeindex>
#include <unordered_map>
#include <memory>

namespace TurnipRenderer {
	class Entity;
	
	class EntityContent : ContextSceneAware {
	public:
		std::string name;

		EntityContent(Entity& entity, Context& context, Scene& scene, std::string name, glm::vec3 localPosition, glm::quat localRotation = glm::quat(1, 0, 0, 0), glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name),
			  transform(entity, localPosition, localRotation, localScale){}
		EntityContent(Entity& entity, Context& context, Scene& scene, std::string name, glm::vec3 localPosition, glm::vec3 localEulerAnglesDegrees, glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name),
			  transform(entity, localPosition, localEulerAnglesDegrees, localScale){}

		void initialize();
		// TODO: Invalidate transform on reparent
		void onReparent() {}

		template<class ComponentType>
		ComponentType* getComponent(){
			static_assert(is_component_type<ComponentType>::value);
			auto valueIter = components.find(typeid(ComponentType));
			if (valueIter == components.end())
				return nullptr; // TODO: Error?
			return reinterpret_cast<ComponentType*>(valueIter->second.get());
		}
		template<class ComponentType, class... Args>
		void addComponent(Args&&... args){
			static_assert(is_component_type<ComponentType>::value);
            // TODO: Error on add already existing
			if (getComponent<ComponentType>() != nullptr) return;
			components[typeid(ComponentType)] = std::make_unique<ComponentType>(std::forward<Args>(args)...);
		}
		
		ResourceHandle<Mesh> mesh;
		bool isOpaque = true;
		glm::vec4 transparencyColor = glm::vec4(1, 0.5f, 0.5f, 0.5f);
		
		Transform transform;
	private:
		std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	};
}

#include "transform.h"
#include "ecs/scene.h"

namespace TurnipRenderer {
	template<>
	Transform* Entity::getComponent<Transform>();
	template<>
	Scene* Entity::getComponent<Scene>();
};
