#include "system.h"

#include "turniprenderer/context.h"
#include "turniprenderer/entity.h"

void FreeCameraSystem::runOnEntity(TurnipRenderer::Entity* camera){
	glm::vec3 localPosDelta = glm::vec3(
		0,
		0,
		context.getInput().perFrame.mouse.scrollAmount
		);
	if (context.getInput().mouse.leftButton.isActive()){
		localPosDelta += glm::vec3(
			-context.getInput().perFrame.mouse.deltaPos.x / 10.0f,
			context.getInput().perFrame.mouse.deltaPos.y / 10.0f,
			0
		);
	}
	localPosDelta = camera->transform.localRotation() * localPosDelta;
	camera->transform.setLocalPosition(
		camera->transform.localPosition() + localPosDelta
		);
	if (context.getInput().mouse.rightButton.isActive()){
		auto currentEulerAngles = camera->transform.localEulerAnglesDegrees();
		float xRotDelta = context.getInput().perFrame.mouse.deltaPos.y / 10.0f;
		currentEulerAngles.x += xRotDelta;
		float yRotDelta = context.getInput().perFrame.mouse.deltaPos.x / 10.0f;
		currentEulerAngles.y += yRotDelta;
		camera->transform.setLocalEulerAnglesDegrees(currentEulerAngles);	
	}
}
