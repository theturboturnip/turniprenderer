#pragma once

#include "scene.h"
#include "component.h"
#include "context_scene_aware.h"
#include "engine_fwd.h"
#include "private/external/glm.h"

#include "material.h"
#include "mesh.h"
#include "shader.h"
#include "resource.h"
#include "transform.h"

#include <typeindex>
#include <unordered_map>

namespace TurnipRenderer {	
	class Entity : ContextSceneAware, public Scene::NodeBase {
	public:
		std::string name;

		Entity(Context& context, Scene& scene, std::string name, glm::vec3 localPosition, glm::quat localRotation = glm::quat(1, 0, 0, 0), glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name),
			  transform(*this, localPosition, localRotation, localScale){}
		Entity(Context& context, Scene& scene, std::string name, glm::vec3 localPosition, glm::vec3 localEulerAnglesDegrees, glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name),
			  transform(*this, localPosition, localEulerAnglesDegrees, localScale){}

		void initialize() override;
		// TODO: Invalidate transform on reparent
		void onReparent() override {}

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
		ResourceHandle<Material> material;
		ResourceHandle<Shader> shader;
		bool isOpaque = true;
		glm::vec4 transparencyColor = glm::vec4(1, 0.5f, 0.5f, 0.5f);
		
		Transform transform;
	private:
		std::unordered_map<std::type_index, std::unique_ptr<Component>> components;
	};
}

#include "transform.h"
#include "scene.h"

namespace TurnipRenderer {
	template<>
	Transform* Entity::getComponent<Transform>();
	template<>
	Scene* Entity::getComponent<Scene>();
};
