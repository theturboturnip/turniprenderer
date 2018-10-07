#include <iostream>

#include "turniprenderer/context.h"
#include "turniprenderer/light.h"

#include "free_camera/component.h"
#include "free_camera/system.h"

int main(){

	/*std::cout << "\n||| Testing ResourceManager |||\n";
	testResourceManager();
	std::cout << "||| Done Testing ResourceManager |||\n";

	std::cout << "\n||| Testing Scene |||\n";
	testScene();
	std::cout << "||| Done Testing Scene |||\n";*/

	TurnipRenderer::Context context("TEST");
	context.initWindow();
	context.initDemoScene();
	context.scene.camera->addComponent<FreeCameraComponent>();
	context.scene.systems.push_back(std::make_unique<FreeCameraSystem>(context));
	auto* light = context.scene.addObjectToEndOfRoot("Light", glm::vec3(0,0,0), glm::vec3(-90,0,0));
	light->
		addComponent<TurnipRenderer::DirectionalLight>(glm::uvec2(2 * 8192), glm::vec3(1,1,0.9f));
	
	while (!context.renderFrame()) {
		light->transform.setLocalEulerAnglesDegrees(glm::vec3(light->transform.localEulerAnglesDegrees() + glm::vec3(0,0,0)));
	}
			
	return 0;
}
