#pragma once

#include "scene.h"
#include "component.h"
#include "context_scene_aware.h"
#include "engine_fwd.h"
#include "private/external/glm.h"

#include "mesh.h"
#include "resource.h"
#include "transform.h"

namespace TurnipRenderer {	
	class Entity : ContextSceneAware, public Scene::NodeBase {
	public:
		std::string name;

		Entity(Context& context, Scene& scene, std::string name, glm::vec3 localPosition, glm::quat localRotation = glm::quat(1, 0, 0, 0), glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name),
			  transform(
				  *dynamic_cast<Transform*>( // Cast the Component* to a Transform*
					  components.emplace_back( // Add the unique_ptr<Transform> in place
						  std::make_unique<Transform>(*this, localPosition, localRotation, localScale)
						  ).get() // Get the Component* from the emplace_back result
					  )
				  ){}
		Entity(Context& context, Scene& scene, std::string name, glm::vec3 localPosition, glm::vec3 localEulerAnglesDegrees, glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name),
			  transform(
				  *dynamic_cast<Transform*>( // Cast the Component* to a Transform*
					  components.emplace_back( // Add the unique_ptr<Transform> in place
						  std::make_unique<Transform>(*this, localPosition, localEulerAnglesDegrees, localScale)
						  ).get() // Get the Component* from the emplace_back result
					  )
				  ){}

		void initialize() override;
		// TODO: Invalidate transform on reparent
		void onReparent() override {}
		
		ResourceHandle<Mesh> mesh;
		bool isOpaque = true;
		glm::vec4 transparencyColor = glm::vec4(1, 0.5f, 0.5f, 0.5f);
		std::vector<std::unique_ptr<Component>> components;

		Transform& transform; // Will always exist, but 
	};
	
}

#include "transform.h"
