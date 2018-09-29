#include "entity.h"

namespace TurnipRenderer {
	void Transform::updateMatrices() const {
		cachedTransformLocalSpaceFromModelSpace = glm::translate(m_localPosition) * glm::mat4_cast(m_localRotation) * glm::scale(m_localScale);
		cachedTransformWorldSpaceFromModelSpace = (entity.isRoot()) ? cachedTransformLocalSpaceFromModelSpace : entity.getParent().transform.transformWorldSpaceFromModelSpace() * cachedTransformLocalSpaceFromModelSpace;
		cachedWorldRotation = (entity.isRoot()) ? m_localRotation : entity.getParent().transform.worldRotation() * m_localRotation;
	}
	void Transform::invalidateLocal() const {
			localInvalidated = true;
			for (Entity& subentity : entity.subentities()){
				subentity.transform.parentInvalidated = true;
			}
		}

	void Entity::initialize(){
		transform.initialize();
	}

	template<>
	Transform* Entity::getComponent<Transform>(){
		return &transform;
	}
	template<>
	SceneAccessComponent* Entity::getComponent<SceneAccessComponent>(){
		return &sceneAccess;
	}
}
