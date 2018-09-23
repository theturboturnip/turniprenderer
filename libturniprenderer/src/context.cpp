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

		createFramebuffers();

		LogAvailableError();
	}

	void Context::createFramebuffers(){
		auto createColorBuffer = [](GLint internalFormat, GLenum format) -> GLuint {
			GLuint colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, WIDTH, HEIGHT, 0, format, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			return colorBuffer;
		};
		auto createDepthBuffer = []() -> GLuint {
			GLuint renderBuffer;
			glGenRenderbuffers(1, &renderBuffer);
			glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, WIDTH, HEIGHT);
			return renderBuffer;
		};
		auto createFramebuffer = [this](GLuint colorTexture, GLuint depthTexture = 0) -> GLuint {
			GLuint frameBuffer;
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, colorTexture, 0);
			GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
			glDrawBuffers(1, drawBuffers);
			if (depthTexture > 0){
				glBindRenderbuffer(GL_RENDERBUFFER, depthTexture);
				glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthTexture);
			}

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				LogAvailableError();
			return frameBuffer;
		};
		

		renderPassData.postProcessBuffers[0] = createColorBuffer(GL_RGB8, GL_RGB);
		renderPassData.postProcessBuffers[1] = createColorBuffer(GL_RGB8, GL_RGB);
		renderPassData.transparencyColorBucketBuffer = createColorBuffer(GL_RGBA32F, GL_RGBA);
		renderPassData.opaqueDepthBuffer = createDepthBuffer();
		renderPassData.transparencyDepthBuffer = createDepthBuffer();
		
		renderPassData.opaqueFramebuffer = createFramebuffer(renderPassData.colorBuffer, renderPassData.opaqueDepthBuffer);
		renderPassData.postProcessingFramebuffers[0] = createFramebuffer(renderPassData.postProcessBuffers[0]);
		renderPassData.postProcessingFramebuffers[1] = createFramebuffer(renderPassData.postProcessBuffers[1]);
		renderPassData.transparencyBucketingFramebuffer = createFramebuffer(renderPassData.transparencyColorBucketBuffer, renderPassData.transparencyDepthBuffer);
	}

	void Context::initDemoScene(){
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

			quad = resources.addResource(Mesh(quadData));
		}
		
		ResourceHandle<Mesh> planeMesh;
		{
			Mesh::MeshData planeData;
			const auto planeNormal = glm::normalize(glm::vec3(0, 1, 1));
			const auto planeTangent = glm::normalize(glm::vec3(0, 1, -1));
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(-1, -1, -1),
							planeNormal,
							planeTangent,
							glm::vec2(0, 0)
							});
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(1, -1, -1),
							planeNormal,
							planeTangent,
							glm::vec2(1, 0)
							});
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(-1, 1, 1),
							planeNormal,
							planeTangent,
							glm::vec2(0, 1)
							});
			planeData.vertices.push_back(Mesh::Vertex{
						glm::vec3(1, 1, 1),
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
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

layout(location = 0) uniform mat4 MVP;

layout(location = 0) out vec3 vertexColor;

void main() {
    gl_Position = MVP * vec4(position, 1);
    vertexColor = vec3(uv0.x, uv0.y, 1);
}
)", R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 vertexColor;
layout(location = 0) out vec3 color;

void main(){
    color = vertexColor;
}
)"));

		std::string passthroughVertexShader = R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

layout(location = 0) out struct {
    vec2 uv0;
} OUT;

void main(){
    gl_Position = vec4(position.x, position.y, 1, 1);
    OUT.uv0 = uv0;
}
)";
		postProcessPassthrough = resources.addResource(Shader(passthroughVertexShader,
															  R"(
#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) uniform sampler2D tex;

layout(location = 0) in struct {
    vec2 uv0;
} IN;

layout(location = 0) out vec4 color;

void main(){
    color = texture(tex, IN.uv0);
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

		// Draw to opaque framebuffer
		{
			glBindFramebuffer(GL_FRAMEBUFFER, renderPassData.opaqueFramebuffer);
			glViewport(0,0, WIDTH,HEIGHT);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(debugProgram->programId);
			glm::mat4 transformViewFromWorld = glm::inverse(scene.camera->transformLocalSpaceFromModelSpace());
			glm::mat4 MVP = cameraData.getTransformProjectionFromView() * transformViewFromWorld;
			glUniformMatrix4fv(0, 1, GL_FALSE,
							   reinterpret_cast<const GLfloat*>(&MVP));
			for (auto* entity : scene.heirarchy){
				if (entity->mesh){
					drawMesh(*entity->mesh);
				}
			}
		}
		// TODO: Draw to transparency buffer
		{}
		// TODO: Blend transparency buffer onto opaque buffer
		{}
		// Postprocessing Effects
		{
			// TODO: Actual Postprocessing
			int currentPostprocessingBuffer = 0;
			// Draw the final result to the screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0,0, WIDTH,HEIGHT);
			drawQuad(*postProcessPassthrough, renderPassData.postProcessBuffers[currentPostprocessingBuffer]);
		}
		SDL_GL_SwapWindow(sdlWindow);
		return done;
	}

	void Context::drawQuad(Shader& shader, GLuint buffer){
		glUseProgram(shader.programId);
		// Bind the current postprocessing buffer to tex0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, buffer);
		glUniform1i(0, 0); // Bind uniform 0 to texture 0
		drawMesh(*quad);
	}
	void Context::drawMesh(Mesh& mesh){
		glBindVertexArray(mesh.getVAO());
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.getIBO());
		glDrawElements(GL_TRIANGLES, mesh.indices().size(), GL_UNSIGNED_INT, 0);
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
