#include "entity.h"

namespace TurnipRenderer {
	void Transform::updateMatrices(){
		cachedTransformLocalSpaceFromModelSpace = glm::translate(m_localPosition) * glm::mat4_cast(m_localRotation) * glm::scale(m_localScale);
		cachedTransformWorldSpaceFromModelSpace = (entity.isRoot()) ? cachedTransformLocalSpaceFromModelSpace : entity.getParent().transform.transformWorldSpaceFromModelSpace() * cachedTransformLocalSpaceFromModelSpace;
	}
	void Transform::invalidateLocal(){
			localInvalidated = true;
			for (Entity& subentity : entity.subentities()){
				subentity.transform.parentInvalidated = true;
			}
		}

	void Entity::initialize(){
		transform.initialize();
	}
}
