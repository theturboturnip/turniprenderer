#include "ecs/entity.h"

namespace TurnipRenderer {
	void Transform::updateMatrices(){
		cachedTransformLocalSpaceFromModelSpace = glm::translate(m_localPosition) * glm::mat4_cast(m_localRotation) * glm::scale(m_localScale);
		cachedTransformWorldSpaceFromModelSpace = (entity.isRoot()) ? cachedTransformLocalSpaceFromModelSpace : entity.getParent().content->transform.transformWorldSpaceFromModelSpace() * cachedTransformLocalSpaceFromModelSpace;
	}
	void Transform::invalidateLocal(){
			localInvalidated = true;
			for (Entity& subentity : entity.subentities()){
				subentity.content->transform.parentInvalidated = true;
			}
		}

	void Impl::EntityContent::initialize(){
		transform.initialize();
	}

	template<>
	Transform* Impl::EntityContent::getComponent<Transform>(){
		return &transform;
	}
	template<>
	Scene* Impl::EntityContent::getComponent<Scene>(){
		return &scene;
	}
}
