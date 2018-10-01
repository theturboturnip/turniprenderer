#include "dirlight_system.h"

#include <assert.h>

#include "context.h"
#include "bounds.h"

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

		// Prep for the rendering
        // Determine the ortho projection matrix
		glm::quat lightRotation = getComponent<const Transform*>(inputs)->worldRotation();
		Bounds objectBounds;
		// TODO: Does transforming each one individually make the frustum smaller?
		for (Entity* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->isOpaque && entity->mesh) objectBounds.encapsulate( entity->transform.transformWorldSpaceFromModelSpace() * entity->mesh->getBounds());
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
		glUseProgram(context.getDefaultShaders().depthOnlyShader->programId);
		for (auto* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->mesh && entity->isOpaque){
				glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
				glUniformMatrix4fv(0, 1, GL_FALSE,
								   reinterpret_cast<const GLfloat*>(&MVP));

				context.renderer.drawMesh(*entity->mesh);
			}
		}

		// Transparent Draw (ignore order)
		glClearColor(directionalLight.color.r, directionalLight.color.g, directionalLight.color.b,0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);

		glUseProgram(context.getDefaultShaders().transparentColorShader->programId);
		for (auto* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->mesh && !entity->isOpaque){
				glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
				glUniformMatrix4fv(0, 1, GL_FALSE,
								   reinterpret_cast<const GLfloat*>(&MVP));
				glUniform4fv(1, 1, reinterpret_cast<const GLfloat*>(&entity->transparencyColor));

				context.renderer.drawMesh(*entity->mesh);
			}
		}

		context.shadowmapsToUse.push_back(Context::Shadowmap{
				directionalLight.shadowmapColorBuffer,
					directionalLight.shadowmapDepthBuffer,
					transformProjectionFromWorld
					});
	}
};
