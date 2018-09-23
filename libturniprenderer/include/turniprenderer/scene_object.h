#pragma once

#include "scene.h"
#include "context_scene_aware.h"
#include "engine_fwd.h"
#include "glm.h"

#include "mesh.h"
#include "resource.h"

namespace TurnipRenderer {

	class Entity : ContextSceneAware {
		friend class Heirarchy<Entity>;
	public:
		std::string name;
		
		glm::vec3 localPosition;
		glm::quat localRotation;
		glm::vec3 localScale;
		inline glm::mat4 transformLocalSpaceFromModelSpace(){
			return glm::translate(localPosition) /* glm::mat4_cast(localRotation)*/ * glm::scale(localScale);
		}

		Entity(Context& context, Scene& scene, std::string&& name, glm::vec3 localPosition, glm::quat localRotation = glm::quat(), glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), name(name), localPosition(localPosition), localRotation(localRotation), localScale(localScale) {}

		//private:
		Scene::NodeData nodeData;

		ResourceHandle<Mesh> mesh;
		bool isOpaque = true;
		glm::vec4 transparencyColor = glm::vec4(1, 0.5f, 0.5f, 0.5f);
	};
	
}
