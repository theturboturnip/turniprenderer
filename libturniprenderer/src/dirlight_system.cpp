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

			glGenTextures(1, &directionalLight.shadowmapColorBuffer);
			glBindTexture(GL_TEXTURE_2D, directionalLight.shadowmapColorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, directionalLight.shadowmapWidth, directionalLight.shadowmapHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

			glGenTextures(1, &directionalLight.shadowmapDepthBuffer);
			glBindTexture(GL_TEXTURE_2D, directionalLight.shadowmapDepthBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, directionalLight.shadowmapWidth, directionalLight.shadowmapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			
			glGenFramebuffers(1, &directionalLight.shadowmapFramebuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, directionalLight.shadowmapFramebuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, directionalLight.shadowmapColorBuffer, 0);
			GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, drawBuffers);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, directionalLight.shadowmapDepthBuffer, 0);

			assert (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
		}
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

		glBindFramebuffer(GL_FRAMEBUFFER, directionalLight.shadowmapFramebuffer);
		glViewport(0,0, directionalLight.shadowmapWidth, directionalLight.shadowmapHeight);
			// Note: Don't clear to white here otherwise if there's nothing in the scene it will look like the program has crashed
			glClearColor(1,0.5,1,0);

			glDisable(GL_BLEND);
			
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			
			for (auto* entity : getComponent<const SceneAccessComponent*>(inputs)->getScene().heirarchy){
				if (entity->mesh && entity->isOpaque){
					if (entity->shader && entity->material && entity->material->texture){
						glUseProgram(entity->shader->programId);
						glActiveTexture(GL_TEXTURE0);
						glBindTexture(GL_TEXTURE_2D, entity->material->texture->textureId);
						glUniform1i(1, 0); // Bind uniform 0 to texture 0
					}else{
						glUseProgram(context.getDebugShaders().debugOpaqueShader->programId);
					}
					glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
					glUniformMatrix4fv(0, 1, GL_FALSE,
									   reinterpret_cast<const GLfloat*>(&MVP));

					//drawMesh(*entity->mesh);
					glBindVertexArray(entity->mesh->getVAO());
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, entity->mesh->getIBO());
					glDrawElements(GL_TRIANGLES, entity->mesh->indices().size(), GL_UNSIGNED_INT, 0);
				}
			}
	}
};
