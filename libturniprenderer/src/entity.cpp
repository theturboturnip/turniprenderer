#include "scene_object.h"

namespace TurnipRenderer {
	void Entity::Transform::updateMatrices(){
		cachedTransformLocalSpaceFromModelSpace = glm::translate(m_localPosition) /* glm::mat4_cast(localRotation)*/ * glm::scale(m_localScale);
		cachedTransformWorldSpaceFromModelSpace = (entity.nodeData.parent != entity.nodeData.me) ? entity.nodeData.parent->transform.transformWorldSpaceFromModelSpace() * cachedTransformLocalSpaceFromModelSpace : cachedTransformLocalSpaceFromModelSpace;
	}

	void Entity::initialize(){
		transform.initialize();
	}
}
