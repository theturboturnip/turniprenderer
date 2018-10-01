#pragma once

#include "private/external/gl.h"
#include "private/external/glm.h"
#include <SDL.h>

#include <functional>

#include "context_aware.h"
#include "resource_manager.h"
#include "mesh.h"
#include "shader.h"

namespace TurnipRenderer {
	struct TextureConfig {
		glm::uvec2 size;

		struct Format {
			GLint internalFormat;
			GLenum format;
			GLenum dataType;
		} formatInfo;

		struct Filter {
			GLenum min = GL_LINEAR;
			GLenum mag = GL_LINEAR;
		} filtering;

		struct AddressMode {
			AddressMode(GLenum wrapMode)
				: wrapMode(wrapMode), borderColor(0) {}
			AddressMode(glm::vec4 borderColor)
				: wrapMode(GL_CLAMP_TO_BORDER), borderColor(borderColor) {}
			
			GLenum wrapMode; // Same for X,Y,Z
			glm::vec4 borderColor;
		} addressMode;
					 
		struct Compare {
			constexpr Compare() {};
			Compare(GLenum compareMode, GLenum compareFunc)
				: compareMode(compareMode), compareFunc(compareFunc) {}
			GLenum compareMode = GL_NONE;
			GLenum compareFunc = GL_LEQUAL;
		} compare;

		TextureConfig(glm::uvec2 size, Format formatInfo, Filter filtering, GLenum wrapMode, Compare compare = Compare())
			: TextureConfig(size, formatInfo, filtering, AddressMode(wrapMode), compare) {}
		TextureConfig(glm::uvec2 size, Format formatInfo, Filter filtering, glm::vec4 borderColor, Compare compare = Compare())
			: TextureConfig(size, formatInfo, filtering, AddressMode(borderColor), compare) {}
		TextureConfig(glm::uvec2 size, Format formatInfo, Filter filtering, AddressMode addressMode, Compare compare = Compare())
			: size(size),
			  formatInfo(formatInfo),
			  filtering(filtering),
			  addressMode(addressMode),
			  compare(compare) {}
	};

	// TODO: Destructor
	struct AddressableBuffer {
		AddressableBuffer(GLuint handle, TextureConfig config)
			: config(config), handle(handle)  {}
		TextureConfig config;

	private:
		friend class Renderer;

		GLuint handle;
		operator GLuint() const {
			return handle;
		}
	};
	// TODO: Will 'using ColorBuffer = AddressableBuffer;' keep the type safety?
	// If so, then migrate to that.
	struct ColorBuffer : public AddressableBuffer {
		using AddressableBuffer::AddressableBuffer;
	};
	struct DepthBuffer : public AddressableBuffer {
		using AddressableBuffer::AddressableBuffer;
	};
	struct FrameBuffer {
		FrameBuffer(GLuint handle,
					glm::uvec2 size,
					std::vector<ResourceHandle<const ColorBuffer>> colorBuffers,
					ResourceHandle<const DepthBuffer> depthBuffer)
			: size(size), colorBuffers(colorBuffers), depthBuffer(depthBuffer), handle(handle) {}
					
		glm::uvec2 size;

		std::vector<ResourceHandle<const ColorBuffer>> colorBuffers;
		ResourceHandle<const DepthBuffer> depthBuffer;

	private:
		friend class Renderer;
		
		GLuint handle;
		operator GLuint() const {
			return handle;
		}
	};
	
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
		
		void drawMesh(Mesh& mesh);
		void drawFullscreenQuad(Shader& shader, const ResourceHandle<const ColorBuffer>& buffer);
		void drawFullscreenQuadAdvanced(Shader& shader, std::function<void()> bindTextures);

		ResourceHandle<const ColorBuffer> createColorBuffer(TextureConfig);
		ResourceHandle<const DepthBuffer> createDepthBuffer(TextureConfig);
		template<class Container>
		ResourceHandle<const FrameBuffer> createFramebuffer(const Container& colorBuffers, ResourceHandle<const DepthBuffer> depthBuffer = nullptr){
			return createFramebuffer(colorBuffers.data(), colorBuffers.size(), depthBuffer);
		}
		ResourceHandle<const FrameBuffer> createFramebuffer(const ResourceHandle<const ColorBuffer>& colorBuffer, ResourceHandle<const DepthBuffer> depthBuffer = nullptr){
			return createFramebuffer(&colorBuffer, 1, depthBuffer);
		}
		ResourceHandle<const FrameBuffer> createFramebuffer(const ResourceHandle<const ColorBuffer>* colorBuffers, size_t colorBuffersCount, ResourceHandle<const DepthBuffer> depthBuffer = nullptr);

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
		
		SDL_Window* sdlWindow = nullptr;
		SDL_GLContext openGlContext;
		glm::uvec2 windowSize;

		const char* currentOperationMessage = nullptr;
		void checkErrors(const char* possibleErrorContext = nullptr);
	};
}
