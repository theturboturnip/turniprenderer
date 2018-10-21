#pragma once

#include "turniprenderer/external/gl.h"
#include "turniprenderer/external/glm.h"
#include <SDL.h>

#include <functional>

#include "turniprenderer/util/context_aware.h"
#include "turniprenderer/resource_manager.h"
#include "turniprenderer/rendering/mesh.h"
#include "turniprenderer/rendering/material.h"
#include "turniprenderer/rendering/shader.h"
#include "turniprenderer/rendering/buffers.h"
#include "turniprenderer/rendering/texture_config.h"

namespace TurnipRenderer {
	class Renderer : ContextAware {
	public:
		Renderer(Context& context)
			: ContextAware(context) {}
		void initialize(std::string windowName, glm::uvec2 windowSize, int openGlMajor, int openGlMinor);
		~Renderer();

		inline void bindFrameBuffer(const ResourceHandle<const FrameBuffer>& fb){
			bindFrameBuffer(static_cast<GLuint>(*fb), fb->size);
		}
		inline void bindWindowFramebuffer(){
			bindFrameBuffer(0, windowSize);
		}

		inline void bindTextureToSlot(GLenum slot, const ResourceHandle<const ColorBuffer>& cb){
			bindTextureToSlot(slot, static_cast<GLuint>(*cb));
		}
		inline void bindTextureToSlot(GLenum slot, const ResourceHandle<const DepthBuffer>& db){
			bindTextureToSlot(slot, static_cast<GLuint>(*db));
		}

		// TODO: Uniform Binding
		// TODO: Pipelines. Have a Pipeline object which can be used to apply shader-agnostic state all at once, do diffs if you want.

		void bindMaterial(const ResourceHandle<const Material>&);

		// TODO: Take a ResourceHandle<const Mesh>
		void drawMesh(Mesh& mesh);
		void drawFullscreenQuad(ShaderBase* shader, const ResourceHandle<const ColorBuffer>& buffer);
		void drawFullscreenQuadAdvanced(ShaderBase* shader, std::function<void()> bindTextures);

		ResourceHandle<const ColorBuffer> createColorBuffer(TextureConfig);
		void fillColorBuffer(const ResourceHandle<const ColorBuffer>&, std::vector<unsigned char>& data);
		void generateMipmapsForColorBuffer(const ResourceHandle<const ColorBuffer>&);
		
		ResourceHandle<const DepthBuffer> createDepthBuffer(TextureConfig);
		template<class Container>
		ResourceHandle<const FrameBuffer> createFramebuffer(const Container& colorBuffers, ResourceHandle<const DepthBuffer> depthBuffer = nullptr){
			return createFramebuffer(colorBuffers.data(), colorBuffers.size(), depthBuffer);
		}
		ResourceHandle<const FrameBuffer> createFramebuffer(const ResourceHandle<const ColorBuffer>& colorBuffer, ResourceHandle<const DepthBuffer> depthBuffer = nullptr){
			return createFramebuffer(&colorBuffer, 1, depthBuffer);
		}
		ResourceHandle<const FrameBuffer> createFramebuffer(const ResourceHandle<const ColorBuffer>* colorBuffers, size_t colorBuffersCount, ResourceHandle<const DepthBuffer> depthBuffer = nullptr);

		// TODO: Move to push/pop paradigm?
		inline void setOperation(const char* operationMessage){
			currentOperationMessage = operationMessage;
		}

	private:
		friend class Context;
		void startFrame();
		void endFrame();
		
		void bindFrameBuffer(GLuint framebufferHandle, glm::uvec2 fbSize);
		void bindTextureToSlot(GLenum slot, GLuint textureHandle);
		GLuint createTextureBuffer(const TextureConfig&);
		
		ResourceManager<ColorBuffer, DepthBuffer, FrameBuffer> resources;

		ResourceHandle<Mesh> fullscreenQuad;
		ResourceHandle<ColorBuffer> whiteTexture;
		
		SDL_Window* sdlWindow = nullptr;
		SDL_GLContext openGlContext;
		glm::uvec2 windowSize;

		const char* currentOperationMessage = nullptr;
		void checkErrors(const char* possibleErrorContext = nullptr);
	};
}
