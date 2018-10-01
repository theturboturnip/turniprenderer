#pragma once

#include "private/external/gl.h"
#include "private/external/glm.h"
#include "private/external/sdl.h"

#include "context_aware.h"
#include "resource_manager.h"
#include "mesh.h"

namespace TurnipRenderer {
	struct BufferConfig {
		BufferConfig(glm::ivec2 size, Format format, Filter filtering, GLenum wrapMode, Compare compare = {})
			: BufferConfig(size, format, filtering, AddressMode(wrapMode), compare) {}
		BufferConfig(glm::ivec2 size, Format format, Filter filtering, glm::vec4 borderColor, Compare compare = {})
			: BufferConfig(size, format, filtering, AddressMode(borderColor), compare) {}
		BufferConfig(glm::ivec2 size, Format format, Filter filtering, AddressMode addressMode, Compare compare = {})
			: size(size),
			  format(format),
			  filtering(filtering),
			  addressMode(addressMode),
			  compare(compare) {}

		glm::ivec2 size;

		struct Format {
			GLint internalFormat;
			GLenum format;
			GLenum type;
		} format;

		struct Filter {
			GLenum minFilter;
			GLenum magFilter;
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
			constexpr Compare() = default;
			GLenum compareMode = GL_NONE;
			GLenum compareFunc = GL_LEQUAL;
		} compare;
	};

	struct AddressableBuffer {
		GLuint handle;
		BufferConfig config;

		operator GLuint() const {
			return handle;
		}
	};
	struct ColorBuffer : public AddressableBuffer {};
	struct DepthBuffer : public AddressableBuffer {};
	struct FrameBuffer {
		glm::ivec2 size;

		std::vector<ResourceHandle<const ColorBuffer>> colorBuffers;
		ResourceHandle<const DepthBuffer> depthBuffer;
	};
	
	class Renderer : ContextAware {
	public:
		Renderer(Context& context)
			: ContextAware(context) {}
		void initialize(std::string windowName, size_t windowWidth, size_t windowHeight, int openGlMajor, int openGlMinor);
		~Renderer();

		void bindFrameBuffer(ResourceHandle<const FrameBuffer>&);
		void bindWindowFramebuffer();

		void bindTextureToSlot(GLenum slot, ResourceHandle<const ColorBuffer>&);
		void bindTextureToSlot(GLenum slot, ResourceHandle<const DepthBuffer>&);
		
		void drawMesh(Mesh& mesh);
		void drawFullscreenQuad(Shader& shader, GLuint buffer);
		void drawFullscreenQuadAdvanced(Shader& shader, std::function<void()> bindTextures);

		ResourceHandle<const ColorBuffer> createColorBuffer(BufferConfig config);
		ResourceHandle<const DepthBuffer> createDepthBuffer(BufferConfig);
		template<class Container>
		ResourceHandle<const FrameBuffer> createFramebuffer(Container colorBuffers, GLuint depthBuffer = 0){
			return createFramebuffer(colorBuffers.data(), colorBuffers.size(), depthBuffer);
		}
		ResourceHandle<const FrameBuffer> createFramebuffer(ResourceHandle<const ColorBuffer>* colorBuffers, GLsizet colorBuffersCount, ResourceHandle<const DepthBuffer> depthBuffer = 0);

	private:
		void bindTextureToSlot(GLenum slot, GLuint textureHandle);
		
		ResourceManager<ColorBuffer, DepthBuffer, FrameBuffer> resources;

		ResourceHandle<Mesh> fullscreenQuad;
		
		SDL_Window* sdlWindow = nullptr;
		SDL_GLContext openGlContext;

		void checkErrors();
	};
}
