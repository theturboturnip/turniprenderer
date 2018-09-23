#include "context.h"

#include <cstdio>

#include "mesh.h"

namespace TurnipRenderer{
	Context::Context(std::string name) : name(std::move(name)), scene(*this) {}
	Context::~Context(){
		if (sdlWindow){
			if (openGlContext){
				SDL_GL_DeleteContext(openGlContext);
			}
			SDL_DestroyWindow(sdlWindow);
			SDL_Quit();
		}
	}
	
	void Context::initWindow(){
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
			LogAvailableError();
			return;
		}
    
		SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
		SDL_GL_SetAttribute( SDL_GL_ALPHA_SIZE, 8 );
    
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

		LogAvailableError();
    
		auto flags = SDL_WINDOW_OPENGL;// | SDL_WINDOW_RESIZABLE;
		sdlWindow = SDL_CreateWindow(name.c_str(),
									 SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
									 WIDTH, HEIGHT,
									 flags);
		if (sdlWindow == nullptr){
			LogAvailableError();
			return;
		}

		openGlContext = SDL_GL_CreateContext(sdlWindow);
		if (!openGlContext){
			LogAvailableError();
			return;
		}
    
		int major, minor;
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
		SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
		fprintf(stderr, "Using OpenGL version %d.%d\n", major, minor);

		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		glClearColor(0.0f,0.0f,0.4f,0.0f);
		LogAvailableError();
	}

	void Context::initDemoScene(){
		ResourceHandle<Mesh> planeMesh;
		{
			Mesh::MeshData planeData;
			const auto planeNormal = glm::normalize(glm::vec3(0, 1, 1));
			const auto planeTangent = glm::normalize(glm::vec3(0, 1, -1));
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(-1, 1, -1),
							planeNormal,
							planeTangent,
							glm::vec2(0, 0)
							});
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(1, 1, -1),
							planeNormal,
							planeTangent,
							glm::vec2(1, 0)
							});
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(-1, -1, 1),
							planeNormal,
							planeTangent,
							glm::vec2(0, 1)
							});
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(1, -1, 1),
							planeNormal,
							planeTangent,
							glm::vec2(1, 1)
							});

			{
				planeData.indices.push_back(0);
				planeData.indices.push_back(1);
				planeData.indices.push_back(2);
			}
			{
				planeData.indices.push_back(1);
				planeData.indices.push_back(2);
				planeData.indices.push_back(3);
			}

			planeMesh = resources.addResource(Mesh(planeData));
		}
		scene.addObjectToEndOfRoot("Plane", glm::vec3(0,0,0))->mesh = planeMesh;

		scene.camera = scene.addObjectToEndOfRoot("Camera", glm::vec3(0,0,10));

		cameraData.fovDegrees = 60;
		cameraData.depthMin = 0.1f;
		cameraData.depthMax = 10.f;
		cameraData.updateProjectionMatrix();

		debugProgram = resources.addResource(Shader(R"(
#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec3 position;
layout(location = 1)   in  vec3 normal;
layout(location = 2)    in  vec3 tangent;
layout(location = 3)   in  vec2 uv0;

layout(location = 0) uniform mat4 MVP;

out vec3 vertexColor;

void main() {
    gl_Position = MVP * vec4(position, 1);
    vertexColor = vec3(uv0.x, uv0.y, 1);
}
)", R"(
#version 330 core

in vec3 vertexColor;
out vec3 color;

void main(){
    color = vertexColor;
}
)"));
	}

	void Context::CameraData::updateProjectionMatrix(){
		transformProjectionFromView = glm::perspective(glm::radians(fovDegrees), WIDTH/(1.0f * HEIGHT), depthMin, depthMax);
	}

	bool Context::renderFrame(){
		bool done = false;
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(sdlWindow))
				done = true;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(debugProgram->programId);
		glm::mat4 transformViewFromWorld = glm::inverse(scene.camera->transformLocalSpaceFromModelSpace());
		glm::mat4 MVP = cameraData.getTransformProjectionFromView() * transformViewFromWorld;
		glUniformMatrix4fv(0, 1, GL_FALSE,
						   reinterpret_cast<const GLfloat*>(&MVP));
		for (auto* entity : scene.heirarchy){
			if (entity->mesh){
				Mesh& mesh = *entity->mesh;
				glBindVertexArray(mesh.getVAO());
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBO());
				glDrawElements(GL_TRIANGLES, mesh.indices().size(), GL_UNSIGNED_INT, 0);
			}
		}
		SDL_GL_SwapWindow(sdlWindow);
		return done;
	}

	void Context::LogAvailableError(){
		const char* sdlError = SDL_GetError();
		if (*sdlError != '\0')
			fprintf(stderr, "SDL Error: %s\n", sdlError);
		GLuint glError = glGetError();
		if (glError)
			fprintf(stderr, "OpenGL Error: %d\n", glError);
	}
};
