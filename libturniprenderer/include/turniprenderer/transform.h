#pragma once

#include "component.h"

namespace TurnipRenderer {
	class Transform : public Component {
	public:
		Transform(Entity& entity, glm::vec3 lp, glm::quat lr, glm::vec3 ls)
			// TODO: Proper euler angle init
			: Transform(entity, lp, quatToEuler(lr), ls){}
		Transform(Entity& entity, glm::vec3 lp, glm::vec3 lead, glm::vec3 ls)
			: entity(entity), m_localPosition(lp), m_localEulerAngles(glm::radians(lead)), m_localScale(ls) {
			updateQuatFromEulerAngles();
		}
		void initialize(){
			updateMatrices();
		}

		inline glm::vec3 localPosition() const {
			return m_localPosition;
		}
		inline void setLocalPosition(glm::vec3 newLocalPosition){
			invalidateLocal();
			m_localPosition = newLocalPosition;
		}
		inline glm::quat localRotation() const {
			return m_localRotation;
		}
		inline void setLocalRotation(glm::quat newLocalRotation) {
			setLocalEulerAnglesDegrees(quatToEuler(newLocalRotation));
		}
		inline glm::vec3 localEulerAnglesDegrees() const {
			return glm::degrees(m_localEulerAngles);
		}
		inline void setLocalEulerAnglesDegrees(glm::vec3 newLocalEulerAnglesDegrees){
			invalidateLocal();
			m_localEulerAngles = glm::radians(newLocalEulerAnglesDegrees);
			updateQuatFromEulerAngles();
		}
		inline glm::vec3 localScale(){
			return m_localScale;
		}
		inline void setLocalScale(glm::vec3 newLocalScale){
			invalidateLocal();
			m_localScale = newLocalScale;
		}
		inline glm::quat worldRotation() const {
			updateMatricesIfNecessary();
			return cachedWorldRotation;
		}
		inline glm::mat4 transformWorldSpaceFromModelSpace() const {
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
		inline glm::vec3 quatToEuler(glm::quat quat){
			return glm::eulerAngles(quat);
		}
		inline void updateQuatFromEulerAngles(){
			glm::vec3 normal = applyEulerAngles(glm::vec3(0,0,1));
			glm::vec3 up = applyEulerAngles(glm::vec3(0,1,0));

			m_localRotation = glm::quat(m_localEulerAngles);
		}
		void invalidateLocal() const;
		inline void updateMatricesIfNecessary() const {
			if (localInvalidated || parentInvalidated) updateMatrices();
		}
		void updateMatrices() const; // Recurse up if other parents are dirty
			
		Entity& entity;
		glm::vec3 m_localPosition;
		glm::quat m_localRotation;
		glm::vec3 m_localEulerAngles;
		glm::vec3 m_localScale;
		mutable glm::mat4 cachedTransformLocalSpaceFromModelSpace = glm::mat4(1);
		mutable glm::mat4 cachedTransformWorldSpaceFromModelSpace = glm::mat4(1);
		mutable glm::quat cachedWorldRotation = glm::quat(1,0,0,0);
		mutable bool localInvalidated = false;
		mutable bool parentInvalidated = false;
	};
};
