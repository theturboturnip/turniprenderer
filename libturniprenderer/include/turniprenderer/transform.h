#pragma once

#include "ecs/impl/entity_def.h"
#include "ecs/component.h"

namespace TurnipRenderer {	
	class Transform : public Component {
	public:
		Transform(Entity& entity, glm::vec3 lp, glm::quat lr, glm::vec3 ls)
			// TODO: Proper euler angle init
			: entity(entity), m_localPosition(lp), m_localRotation(lr), m_localEulerAngles(0), m_localScale(ls) {}
		Transform(Entity& entity, glm::vec3 lp, glm::vec3 lead, glm::vec3 ls)
			: entity(entity), m_localPosition(lp), m_localEulerAngles(glm::radians(lead)), m_localScale(ls) {
			updateQuatFromEulerAngles();
		}
		void initialize(){
			updateMatrices();
		}

		inline glm::vec3 localPosition(){
			return m_localPosition;
		}
		inline void setLocalPosition(glm::vec3 newLocalPosition){
			invalidateLocal();
			m_localPosition = newLocalPosition;
		}
		inline glm::quat localRotation(){
			return m_localRotation;
		}
		inline glm::vec3 localEulerAnglesDegrees(){
			return glm::degrees(m_localEulerAngles);
		}
		inline void setLocalEulerAnglesDegrees(glm::vec3 newLocalEulerAnglesDegrees){
			invalidateLocal();
			m_localEulerAngles = glm::radians(newLocalEulerAnglesDegrees);
			updateQuatFromEulerAngles();
		}
		inline void setLocalScale(glm::vec3 newLocalScale){
			invalidateLocal();
			m_localScale = newLocalScale;
		}
		inline const glm::mat4 transformWorldSpaceFromModelSpace(){
			updateMatricesIfNecessary();
			return cachedTransformWorldSpaceFromModelSpace;
		}
	private:
		inline glm::vec3 applyEulerAngles(glm::vec3 applyTo){
			applyTo = glm::rotateZ(applyTo, m_localEulerAngles.z);
			applyTo = glm::rotateX(applyTo, m_localEulerAngles.x);
			applyTo = glm::rotateY(applyTo, m_localEulerAngles.y);
			return applyTo;
		}
		inline void updateQuatFromEulerAngles(){
			glm::vec3 normal = applyEulerAngles(glm::vec3(0,0,1));
			glm::vec3 up = applyEulerAngles(glm::vec3(0,1,0));

			m_localRotation = glm::quat(m_localEulerAngles);
		}
		void invalidateLocal();
		inline void updateMatricesIfNecessary(){
			if (localInvalidated || parentInvalidated) updateMatrices();
		}
		void updateMatrices(); // Recurse up if other parents are dirty
			
		Entity& entity;
		glm::vec3 m_localPosition;
		glm::quat m_localRotation;
		glm::vec3 m_localEulerAngles;
		glm::vec3 m_localScale;
		glm::mat4 cachedTransformLocalSpaceFromModelSpace = glm::mat4(1);
		glm::mat4 cachedTransformWorldSpaceFromModelSpace = glm::mat4(1);
		bool localInvalidated = false;
		bool parentInvalidated = false;
	};
};
