#include "renderer.h"

#include "context.h"

#include <sstream>

namespace TurnipRenderer {
	void Renderer::initialize(std::string windowName, glm::uvec2 windowSize, int openGlMajor, int openGlMinor){
		this->windowSize = windowSize;
		
		if (SDL_Init(SDL_INIT_VIDEO) != 0){
			checkErrors("SDL Init");
		}
    
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, openGlMajor);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, openGlMinor);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		checkErrors("OpenGL Pre-Creation Setup");
    
		auto flags = SDL_WINDOW_OPENGL;// | SDL_WINDOW_RESIZABLE;
		sdlWindow = SDL_CreateWindow(windowName.c_str(),
									 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									 windowSize.x, windowSize.y,
									 flags);
		if (sdlWindow == nullptr){
			checkErrors("SDL Window Creation"); // Will throw
		}

		openGlContext = SDL_GL_CreateContext(sdlWindow);
		if (!openGlContext){
			checkErrors("OpenGL Context Creation");
		}
    
		int major, minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		fprintf(stderr, "Using OpenGL version %d.%d\n", major, minor);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		checkErrors("OpenGL Post-Creation Setup");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplSDL2_InitForOpenGL(sdlWindow, openGlContext);
		ImGui_ImplOpenGL3_Init("#version 150");
		checkErrors("ImGui Setup");

		// Create Quad Mesh
		{
			Mesh::MeshData quadData;
			quadData.vertices.push_back(Mesh::Vertex{
					glm::vec3(-1, -1, 0),
						glm::vec3(0),
						glm::vec3(0),
						glm::vec2(0, 0)
						});
			quadData.vertices.push_back(Mesh::Vertex{
					glm::vec3(1, -1, 0),
						glm::vec3(0),
						glm::vec3(0),
						glm::vec2(1, 0)
						});
			quadData.vertices.push_back(Mesh::Vertex{
					glm::vec3(-1, 1, 0),
						glm::vec3(0),
						glm::vec3(0),
						glm::vec2(0, 1)
						});
			quadData.vertices.push_back(Mesh::Vertex{
					glm::vec3(1, 1, 0),
						glm::vec3(0),
						glm::vec3(0),
						glm::vec2(1, 1)
						});

			{
				quadData.indices.push_back(0);
				quadData.indices.push_back(1);
				quadData.indices.push_back(2);
			}
			{
				quadData.indices.push_back(1);
				quadData.indices.push_back(2);
				quadData.indices.push_back(3);
			}

			fullscreenQuad = context.resources.addResource(Mesh(std::move(quadData)));
		}
	}
	Renderer::~Renderer(){
		if (sdlWindow){
			if (openGlContext){
				ImGui_ImplOpenGL3_Shutdown();
				ImGui::DestroyContext();
				
				SDL_GL_DeleteContext(openGlContext);
			}
			SDL_DestroyWindow(sdlWindow);
			SDL_Quit();
		}
	}

	void Renderer::bindFrameBuffer(GLuint framebufferHandle, glm::uvec2 fbSize){
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferHandle);
		glViewport(0, 0, fbSize.x, fbSize.y);
		checkErrors("bindFramebuffer");
	}
	void Renderer::bindTextureToSlot(GLenum slot, GLuint textureHandle) {
		glActiveTexture(slot);
		glBindTexture(GL_TEXTURE_2D, textureHandle);
		checkErrors("bindTextureToSlot");
	}

	void Renderer::drawFullscreenQuad(Shader& shader, const ResourceHandle<const ColorBuffer>& buffer){
		drawFullscreenQuadAdvanced(shader, [this, buffer](){
				// Bind the current postprocessing buffer to tex0
				bindTextureToSlot(GL_TEXTURE0, buffer);
				glUniform1i(0, 0); // Bind uniform 0 to texture 0
			});
	}
	void Renderer::drawFullscreenQuadAdvanced(Shader& shader, std::function<void()> bindTextures){
		glUseProgram(shader.programId);
		checkErrors("drawFullscreenQuadAdvanced - Shader Bind");
		bindTextures();
		drawMesh(*fullscreenQuad);
	}
	void Renderer::drawMesh(Mesh& mesh){
		glBindVertexArray(mesh.getVAO());
		checkErrors("drawMesh - bindVertexArray");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBO());
		checkErrors("drawMesh - bindBuffer");
		glDrawElements(GL_TRIANGLES, mesh.indices().size(), GL_UNSIGNED_INT, 0);
		checkErrors("drawMesh - drawElements");
	}

	ResourceHandle<const ColorBuffer> Renderer::createColorBuffer(TextureConfig config){
		config.compare = TextureConfig::Compare(); // Disable comparisons
		return resources.addResource(ColorBuffer(
										 createTextureBuffer(config),
										 config
										 ));
	}
	ResourceHandle<const DepthBuffer> Renderer::createDepthBuffer(TextureConfig config){
		config.formatInfo = {
			GL_DEPTH_COMPONENT,
			GL_DEPTH_COMPONENT,
			GL_FLOAT
		};
		return resources.addResource(DepthBuffer(
										 createTextureBuffer(config),
										 config
										 ));
	}
	GLuint Renderer::createTextureBuffer(const TextureConfig& config){
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		checkErrors("createTextureBuffer - Creation");

		glTexImage2D(GL_TEXTURE_2D, 0, config.formatInfo.internalFormat, config.size.x, config.size.y, 0, config.formatInfo.format, config.formatInfo.dataType, nullptr);
		checkErrors("createTextureBuffer - Formatting");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, config.filtering.min);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, config.filtering.mag);
		checkErrors("createTextureBuffer - Filtering");

		if (config.compare.compareMode != GL_NONE) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, config.compare.compareMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, config.compare.compareFunc);
		}
		checkErrors("createTextureBuffer - Comparison");
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, config.addressMode.wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, config.addressMode.wrapMode);
		if (config.addressMode.wrapMode == GL_CLAMP_TO_BORDER)
			glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
							 reinterpret_cast<const float*>(&config.addressMode.borderColor));
		checkErrors("createTextureBuffer - Wrap/Border");
		
		return texture;
	}
	ResourceHandle<const FrameBuffer> Renderer::createFramebuffer(const ResourceHandle<const ColorBuffer>* colorBuffers, size_t colorBuffersCount, ResourceHandle<const DepthBuffer> depthBuffer){
		GLuint frameBuffer;
		glGenFramebuffers(1, &frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

		assert(colorBuffersCount <= 8);
		std::vector<GLenum> drawBuffers;
		for (auto i = 0u; i < colorBuffersCount; i++){
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, *colorBuffers[i], 0);
			drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + i);
		}
		glDrawBuffers(drawBuffers.size(), drawBuffers.data());
		checkErrors("createFramebuffer - drawBuffers");
		if (depthBuffer)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, *depthBuffer, 0);
		checkErrors("createFramebuffer");
		assert (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		return resources.addResource(FrameBuffer(
										 frameBuffer,
										 (colorBuffersCount ? colorBuffers[0]->config.size : depthBuffer->config.size),
										 std::vector<ResourceHandle<const ColorBuffer>>(colorBuffers, colorBuffers + colorBuffersCount),
										 depthBuffer
										 ));
	}

	void Renderer::startFrame(){
	}
	void Renderer::endFrame(){
		SDL_GL_SwapWindow(sdlWindow);
	}
	
	void Renderer::checkErrors(const char* possibleErrorContext){
		const char* sdlError = SDL_GetError();
		GLuint glError = glGetError();
		if (!glError && (!sdlError || *sdlError == '\0')) return;

		std::stringstream errorMsg;
		
		if (glError){
			errorMsg << "OpenGL Error: ";
			const char* customMsg = nullptr;
			switch(glError){
			case 0x0500:
				errorMsg << "GL_INVALID_ENUM";
				break;
			case 0x0501:
				errorMsg << "GL_INVALID_VALUE";
				break;
			case 0x0502:
				errorMsg << "GL_INVALID_OPERATION";
				break;
			case 0x0503:
				errorMsg << "GL_STACK_OVERFLOW";
				break;
			case 0x0504:
				errorMsg << "GL_STACK_UNDERFLOW";
				break;
			case 0x0505:
				errorMsg << "GL_OUT_OF_MEMORY";
				break;
			default:
				errorMsg << (int)glError;
			}
			errorMsg << '\n';
		}
		if (sdlError && *sdlError != '\0'){
			errorMsg << "SDL Error: " << sdlError << '\n';
		}

		if (currentOperationMessage)
			errorMsg << "During operation: " << currentOperationMessage << '\n';
		if (possibleErrorContext)
			errorMsg << "Internal Error Context: " << possibleErrorContext << '\n';

		throw std::runtime_error(errorMsg.str());
	}
};
