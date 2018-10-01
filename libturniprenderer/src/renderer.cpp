#include "renderer.h"

#include "context.h"

namespace TurnipRenderer {
	void Renderer::initialize(std::string windowName, size_t windowWidth, size_t windowHeight, int openGlMajor, int openGlMinor){
		if (SDL_Init(SDL_INIT_VIDEO) != 0){
			LogAvailableError();
			return;
		}
    
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, openglMajor);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, openglMinor);
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		LogAvailableError();
    
		auto flags = SDL_WINDOW_OPENGL;// | SDL_WINDOW_RESIZABLE;
		sdlWindow = SDL_CreateWindow(name.c_str(),
									 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									 windowWidth, windowHeight,
									 flags);
		if (sdlWindow == nullptr){
			checkErrors(); // Will throw
		}

		openGlContext = SDL_GL_CreateContext(sdlWindow);
		if (!openGlContext){
			checkErrors();
		}
    
		int major, minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		fprintf(stderr, "Using OpenGL version %d.%d\n", major, minor);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		checkErrors();

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

	// TODO: Update to use the new error-checked systems
	void Renderer::drawFullscreenQuad(Shader& shader, GLuint buffer){
		drawFullscreenQuadAdvanced(shader, [buffer](){
				// Bind the current postprocessing buffer to tex0
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, buffer);
				glUniform1i(0, 0); // Bind uniform 0 to texture 0
			});
	}
	void Renderer::drawFullscreenQuadAdvanced(Shader& shader, std::function<void()> bindTextures){
		glUseProgram(shader.programId);
		bindTextures();
		drawMesh(*quad);
	}
	// TODO: Error check
	void Renderer::drawMesh(Mesh& mesh){
		glBindVertexArray(mesh.getVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBO());
		glDrawElements(GL_TRIANGLES, mesh.indices().size(), GL_UNSIGNED_INT, 0);
	}

	void Renderer::checkErrors(){
		const char* sdlError = SDL_GetError();
		if (sdlError && *sdlError != '\0')
			throw std:::runtime_error(sdlError);
		GLuint glError = glGetError();
		if (glError){
			const char* msg = nullptr;
			switch(glError){
			case 1282:
				msg = "1282";
			}
			if (msg)
				throw std::runtime_error(std::string("OpenGL Error ") + msg);
			else
				throw std::runtime_error(std::string("OpenGL Error ") + (int)glError);
		}
	}
};
