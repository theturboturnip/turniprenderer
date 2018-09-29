#include "dirlight_system.h"

#include <assert.h>

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
		
		// Render to buffer
	}
};
