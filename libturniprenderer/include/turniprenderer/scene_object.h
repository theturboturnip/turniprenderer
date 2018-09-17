#pragma once

#include "scene.h"
#include "context_scene_aware.h"

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"

namespace TurnipRenderer {

	class SceneObject : ContextSceneAware {
		friend class Heirarchy<SceneObject>;
		Scene::NodeData nodeData;
	public:
			glm::vec3 localPosition;
			glm::quat localRotation;
			glm::vec3 localScale;
			inline glm::mat4 transformLocalSpaceFromModelSpace(){
				return glm::mat4();
			}
	   

		SceneObject(Context& context, Scene& scene, glm::vec3 localPosition, glm::quat localRotation = glm::quat(), glm::vec3 localScale = glm::vec3(1))
			: ContextSceneAware(context, scene), localPosition(localPosition), localRotation(localRotation), localScale(localScale) {}
	};
	
}
