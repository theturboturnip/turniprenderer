#include "system.h"

#include "turniprenderer/context.h"
#include "turniprenderer/entity.h"

void FreeCameraSystem::runOnEntity(TurnipRenderer::Entity* shouldBeUnused, const System::Inputs inputs, const System::Outputs outputs){
	// TODO: Use the data in the input
	TurnipRenderer::Transform& transform = *std::get<TurnipRenderer::Transform*>(outputs);
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
	localPosDelta = transform.localRotation() * localPosDelta;
	transform.setLocalPosition(
		transform.localPosition() + localPosDelta
		);
	if (context.getInput().mouse.rightButton.isActive()){
		auto currentEulerAngles = transform.localEulerAnglesDegrees();
		float xRotDelta = context.getInput().perFrame.mouse.deltaPos.y / 10.0f;
		currentEulerAngles.x += xRotDelta;
		float yRotDelta = context.getInput().perFrame.mouse.deltaPos.x / 10.0f;
		currentEulerAngles.y += yRotDelta;
		transform.setLocalEulerAnglesDegrees(currentEulerAngles);	
	}
}
