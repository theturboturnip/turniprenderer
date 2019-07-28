#include "turniprenderer/dirlight_system.h"

#include <assert.h>

#include "turniprenderer/context.h"
#include "turniprenderer/util/bounds.h"

namespace TurnipRenderer {
	void DirectionalLightRenderer::runOnEntity(Entity* entity, const System::Inputs inputs, const System::Outputs outputs){
		auto& directionalLight = *getComponent<DirectionalLight*>(outputs);
		if (!directionalLight.initialized){
			// Create OpenGL things
			directionalLight.initialized = true;

			context.renderer.setOperation("DirLight Shadowmap Color Buffer");
			directionalLight.shadowmapColorBuffer = context.renderer.createColorBuffer(
				TextureConfig(
					directionalLight.shadowmapSize,
					{ GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE },
					{ GL_LINEAR, GL_LINEAR },
					glm::vec4(directionalLight.color, 1)
					)
				);

			context.renderer.setOperation("DirLight Shadowmap Depth Buffer");
			directionalLight.shadowmapDepthBuffer = context.renderer.createDepthBuffer(
				TextureConfig(
					directionalLight.shadowmapSize,
					{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE },
					{ GL_LINEAR, GL_LINEAR },
					glm::vec4(1,1,1,1),
					{ GL_COMPARE_REF_TO_TEXTURE, GL_LEQUAL }
					)
				);
			
			context.renderer.setOperation("DirLight Shadowmap FrameBuffer");
			directionalLight.shadowmapFramebuffer = context.renderer.createFramebuffer(
				directionalLight.shadowmapColorBuffer,
				directionalLight.shadowmapDepthBuffer
				);
		}

		// Prep for rendering
		// Determine the ortho matrix
		glm::quat lightRotation = getComponent<const Transform*>(inputs)->worldRotation();
		Bounds objectBounds;
		// TODO: Does transforming each one individually make the frustum smaller?
		for (Entity* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->renderable()) objectBounds.encapsulate( entity->transform.transformWorldSpaceFromModelSpace() * entity->mesh->getBounds());
		}
		objectBounds = glm::inverse(lightRotation) * objectBounds;
		glm::vec3 extents = objectBounds.getExtents();
		glm::mat4 transformProjectionFromView = glm::ortho(-extents.x, extents.x, -extents.y, extents.y, -extents.z, extents.z);
		glm::mat4 transformViewFromWorld = glm::inverse( glm::mat4_cast(lightRotation) * glm::translate(objectBounds.getCentre()) );
		glm::mat4 transformProjectionFromWorld = transformProjectionFromView * transformViewFromWorld;

		// Render to buffer
		context.renderer.setOperation("DirLight Shadowmap Rendering");
		context.renderer.bindFrameBuffer(directionalLight.shadowmapFramebuffer);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
			
		glClear(GL_DEPTH_BUFFER_BIT);

		// Opaque Draw
		context.renderer.setOperation("DirLight Shadowmap Opaque Rendering");
		glUseProgram(context.getDefaultShaders().depthOnlyShader->programId);
		for (auto* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->renderable() && entity->material->isOpaque()){
				glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
				glUniformMatrix4fv(0, 1, GL_FALSE,
								   reinterpret_cast<const GLfloat*>(&MVP));

				context.renderer.drawMesh(*entity->mesh);
			}
		}

		// Transparent Draw (ignore order)
		context.renderer.setOperation("DirLight Shadowmap Transparent Rendering");
		glClearColor(directionalLight.color.r, directionalLight.color.g, directionalLight.color.b,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glUseProgram(context.getDefaultShaders().phongTransparentNoShadowShader->programId);
		for (auto* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->renderable() && entity->material->isTranslucent() && entity->shader){
				glm::mat4 M = entity->transform.transformWorldSpaceFromModelSpace();
				glm::mat4 MVP = transformProjectionFromWorld * M;
									
					context.renderer.bindMaterial(entity->material);
					
					//glm::mat4 lightMVP;
					glm::vec3 lightDirection = lightRotation * glm::vec4(0,0,1,0);
					/*if (shadowmapsToUse.size() > 0){
						renderer.bindTextureToSlot(GL_TEXTURE1, shadowmapsToUse[0].colorBuffer);
						glUniform1i(3, 1);
						renderer.bindTextureToSlot(GL_TEXTURE2, shadowmapsToUse[0].depthBuffer);
						glUniform1i(4, 2);
						lightMVP = shadowmapsToUse[0].VP * M;
					lightDirection = glm::inverse(shadowmapsToUse[0].V) * glm::vec4(0,0,1,0);
					}*/
					
					glUniformMatrix4fv(1, 1, GL_FALSE,
									   reinterpret_cast<const GLfloat*>(&M));
					glUniform3fv(3, 1, reinterpret_cast<const GLfloat*>(&directionalLight.color));
					/*glUniformMatrix4fv(2, 1, GL_FALSE,
					  reinterpret_cast<const GLfloat*>(&lightMVP));*/
					glm::vec3 cameraPos = glm::inverse(transformViewFromWorld) * glm::vec4(0,0,0,1);
					glUniform3fv(5, 1,
								 reinterpret_cast<const GLfloat*>(&cameraPos));
					glUniform3fv(6, 1,
								 reinterpret_cast<const GLfloat*>(&lightDirection));
				
					glUniformMatrix4fv(0, 1, GL_FALSE,
									   reinterpret_cast<const GLfloat*>(&MVP));
					
					context.renderer.drawMesh(*entity->mesh);
			}
		}

		context.shadowmapsToUse.push_back(Shadowmap{
				directionalLight.shadowmapColorBuffer,
					directionalLight.shadowmapDepthBuffer,
					transformProjectionFromWorld,
					transformViewFromWorld
					});
	}
};
