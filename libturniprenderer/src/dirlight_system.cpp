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
			/*glGenTextures(1, &directionalLight.shadowmapColorBuffer);
			glBindTexture(GL_TEXTURE_2D, directionalLight.shadowmapColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, directionalLight.shadowmapWidth, directionalLight.shadowmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			context.LogAvailableError();
			fprintf(stderr, "Created the relevant color textures\n");*/

			context.renderer.setOperation("DirLight Shadowmap Depth Buffer");
			directionalLight.shadowmapDepthBuffer = context.renderer.createDepthBuffer(
				TextureConfig(
					directionalLight.shadowmapSize,
					{ GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE },
					{ GL_LINEAR, GL_LINEAR },
					glm::vec4(1,1,1,1),
					/*TextureConfig::Compare*/{ GL_COMPARE_REF_TO_TEXTURE, GL_LEQUAL }
					)
				);
			/*glGenTextures(1, &directionalLight.shadowmapDepthBuffer);
			glBindTexture(GL_TEXTURE_2D, directionalLight.shadowmapDepthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, directionalLight.shadowmapWidth, directionalLight.shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			float borderDepth[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderDepth);

			context.LogAvailableError();*/
			//fprintf(stderr, "Created the relevant textures\n");
			
			context.renderer.setOperation("DirLight Shadowmap FrameBuffer");
			directionalLight.shadowmapFramebuffer = context.renderer.createFramebuffer(
				directionalLight.shadowmapColorBuffer,
				directionalLight.shadowmapDepthBuffer
				);
			/*glGenFramebuffers(1, &directionalLight.shadowmapFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, directionalLight.shadowmapFramebuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, directionalLight.shadowmapColorBuffer, 0);
			GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, drawBuffers);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalLight.shadowmapDepthBuffer, 0);
			assert (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			context.LogAvailableError();*/
			//fprintf(stderr, "Created the relevant framebuffers\n");
				
		}
		context.renderer.setOperation("DirLight Shadowmap Rendering");
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

		context.renderer.bindFrameBuffer(directionalLight.shadowmapFramebuffer);

		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
			
		glClear(GL_DEPTH_BUFFER_BIT);

		//context.LogAvailableError();
		//fprintf(stderr, "Started the opaque draw to the shadowmap\n");

		// Opaque Draw
		glUseProgram(context.getDefaultShaders().depthOnlyShader->programId);
		for (auto* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
			if (entity->mesh && entity->isOpaque){
				/*if (entity->shader && entity->material && entity->material->texture){
				  glUseProgram(entity->shader->programId);
				  glActiveTexture(GL_TEXTURE0);
				  glBindTexture(GL_TEXTURE_2D, entity->material->texture->textureId);
				  glUniform1i(1, 0); // Bind uniform 0 to texture 0
				  }else{*/
				
				//}
				glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
				glUniformMatrix4fv(0, 1, GL_FALSE,
								   reinterpret_cast<const GLfloat*>(&MVP));

				context.renderer.drawMesh(*entity->mesh);
			}
		}

		//context.LogAvailableError();
		//fprintf(stderr, "Finished the opaque draw to the shadowmap\n");

		// Transparent Draw (ignore order)
		//glBindFramebuffer(GL_FRAMEBUFFER, directionalLight.shadowmapFramebuffer);
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
