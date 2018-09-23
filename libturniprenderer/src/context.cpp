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
		if (SDL_Init(SDL_INIT_VIDEO) != 0){
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
		auto createColorBuffer = [this](GLint internalFormat, GLenum format, GLenum type = GL_FLOAT) -> GLuint {
			GLuint colorBuffer;
			glGenTextures(1, &colorBuffer);
			glBindTexture(GL_TEXTURE_2D, colorBuffer);
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, WIDTH, HEIGHT, 0, format, type, nullptr);
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
		auto createTransparencyFramebuffer = [this]() -> GLuint {
			GLuint frameBuffer;
			glGenFramebuffers(1, &frameBuffer);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderPassData.transparencyColorBucketBuffers[0], 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, renderPassData.transparencyColorBucketBuffers[1], 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, renderPassData.transparencyColorBucketBuffers[2], 0);
			glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, renderPassData.transparencyColorBucketBuffers[3], 0);

			GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
			glDrawBuffers(4, drawBuffers);
			glBindRenderbuffer(GL_RENDERBUFFER, renderPassData.transparencyDepthBuffer);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderPassData.transparencyDepthBuffer);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				LogAvailableError();
			return frameBuffer;
		};

		renderPassData.postProcessBuffers[0] = createColorBuffer(GL_RGB8, GL_RGB);
		renderPassData.postProcessBuffers[1] = createColorBuffer(GL_RGB8, GL_RGB);
		renderPassData.transparencyColorBucketBuffers[0] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.transparencyColorBucketBuffers[1] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.transparencyColorBucketBuffers[2] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.transparencyColorBucketBuffers[3] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.opaqueDepthBuffer = createDepthBuffer();
		renderPassData.transparencyDepthBuffer = renderPassData.opaqueDepthBuffer;//createDepthBuffer();
		
		renderPassData.opaqueFramebuffer = createFramebuffer(renderPassData.colorBuffer, renderPassData.opaqueDepthBuffer);
		renderPassData.postProcessingFramebuffers[0] = createFramebuffer(renderPassData.postProcessBuffers[0]);
		renderPassData.postProcessingFramebuffers[1] = createFramebuffer(renderPassData.postProcessBuffers[1]);
		renderPassData.transparencyBucketingFramebuffer = createTransparencyFramebuffer();
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
		
		/*ResourceHandle<Mesh> planeMesh;
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

		auto* plane1 = scene.addObjectToEndOfRoot("Transparent Plane #1", glm::vec3(1,0,2));
		plane1->mesh = quad;
		plane1->isOpaque = false;
		plane1->transparencyColor = glm::vec4(1, 0.1, 0.1, 0.5);
		auto* plane2 = scene.addObjectToEndOfObject(*plane1, "Transparent Plane #2", glm::vec3(0.25,0,2));
		plane2->mesh = quad;
		//plane2->isOpaque = false;
		plane2->transparencyColor = glm::vec4(0.1, 1, 0.1, 0.5);
		auto* plane3 = scene.addObjectToEndOfObject(*plane2, "Transparent Plane #3", glm::vec3(0.25,0,2));
		plane3->mesh = quad;
		plane3->isOpaque = false;
		plane3->transparencyColor = glm::vec4(0.1, 0.1, 1, 0.5);*/

		scene.addModel("assets/sponza/sponza.fbx");

		scene.camera = scene.addObjectToEndOfRoot("Camera", glm::vec3(0,0,10));

		cameraData.fovDegrees = 60;
		cameraData.depthMin = 0.1f;
		cameraData.depthMax = 10.f;
		cameraData.updateProjectionMatrix();

		debugOpaqueProgram = resources.addResource(Shader(R"(
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

		debugTransparentProgram = resources.addResource(Shader(R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 uv0;

layout(location = 0) uniform mat4 MVP;
layout(location = 1) uniform float depthMin;
layout(location = 2) uniform float depthMax;

layout(location = 0) out float depth;

void main() {
    gl_Position = MVP * vec4(position, 1);
    depth = (gl_Position.z - depthMin) / (depthMax - depthMin);
}
)", R"(
#version 330 core
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_uniform_location : enable

layout(location = 3) uniform vec4 transparencyColor;

layout(location = 0) in float depth;
layout(location = 0) out vec4 bucket0;
layout(location = 1) out vec4 bucket1;
layout(location = 2) out vec4 bucket2;
layout(location = 3) out vec4 bucket3;

void main(){
    uint bucketIndex = uint(clamp(depth, 0.0f, 1.0f) * 4u);
    if (bucketIndex > 3u) bucketIndex = 3u;
    bucket0 = (bucketIndex == 0u) ? transparencyColor : vec4(0);
    bucket1 = (bucketIndex == 1u) ? transparencyColor : vec4(0);
    bucket2 = (bucketIndex == 2u) ? transparencyColor : vec4(0);
    bucket3 = (bucketIndex == 3u) ? transparencyColor : vec4(0);
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
color = vec4(texture(tex, IN.uv0));
}
)"));

		transparencyResolve = resources.addResource(Shader(passthroughVertexShader,
															  R"(
#version 330 core
#extension GL_ARB_explicit_uniform_location : enable
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) uniform sampler2D bucket0;
layout(location = 1) uniform sampler2D bucket1;
layout(location = 2) uniform sampler2D bucket2;
layout(location = 3) uniform sampler2D bucket3;

layout(location = 0) in struct {
    vec2 uv0;
} IN;

layout(location = 0) out vec4 color;

void main(){
    vec4 values[4] = vec4[]( texture(bucket0, IN.uv0), texture(bucket1, IN.uv0), texture(bucket2, IN.uv0), texture(bucket3, IN.uv0) );
    color = vec4(0);
    //for (int i = 0; i < 4; i++){
    for (int i = 3; i >= 0; i--){
        float srcAlpha = values[i].a;
        color = vec4(color.rgb * (1 - srcAlpha) + values[i].rgb * srcAlpha, color.a + values[i].a);
    }
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

		glm::mat4 transformViewFromWorld = glm::inverse(scene.camera->transform.transformWorldSpaceFromModelSpace());
		glm::mat4 transformProjectionFromWorld = cameraData.getTransformProjectionFromView() * transformViewFromWorld;
		
		// Draw to opaque framebuffer
		{
			glBindFramebuffer(GL_FRAMEBUFFER, renderPassData.opaqueFramebuffer);
			glViewport(0,0, WIDTH,HEIGHT);
			// Note: Don't clear to white here otherwise if there's nothing in the scene it will look like the program has crashed
			glClearColor(1,0.5,1,0);

			glDisable(GL_BLEND);
			
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(debugOpaqueProgram->programId);
			
			for (auto* entity : scene.heirarchy){
				if (entity->mesh && entity->isOpaque){
					glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
					glUniformMatrix4fv(0, 1, GL_FALSE,
							   reinterpret_cast<const GLfloat*>(&MVP));
					drawMesh(*entity->mesh);
				}
			}
		}
		// Draw to transparency buffer
		{
			glBindFramebuffer(GL_FRAMEBUFFER, renderPassData.transparencyBucketingFramebuffer);
			glViewport(0,0, WIDTH,HEIGHT);
			glClearColor(0,0,0,0);
			glClear(GL_COLOR_BUFFER_BIT);
			glUseProgram(debugTransparentProgram->programId);

			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			
			glUniform1f(1, cameraData.depthMin);
			glUniform1f(2, cameraData.depthMax);

			for (auto* entity : scene.heirarchy){
				if (entity->mesh && !entity->isOpaque){
					glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
					glUniformMatrix4fv(0, 1, GL_FALSE,
									   reinterpret_cast<const GLfloat*>(&MVP));
					glUniform4fv(3, 1, reinterpret_cast<const GLfloat*>(&entity->transparencyColor));
					drawMesh(*entity->mesh);
				}
			}
		}
		// Blend transparency buffer onto opaque buffer
		{
			glBindFramebuffer(GL_FRAMEBUFFER, renderPassData.opaqueFramebuffer);
			glViewport(0,0, WIDTH,HEIGHT);
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			drawQuadAdvanced(*transparencyResolve, [this]() {
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, renderPassData.transparencyColorBucketBuffers[0]);
					glUniform1i(0, 0); // Bind uniform 0 to texture 0
					
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(GL_TEXTURE_2D, renderPassData.transparencyColorBucketBuffers[1]);
					glUniform1i(1, 1); // Bind uniform 0 to texture 0
					
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, renderPassData.transparencyColorBucketBuffers[2]);
					glUniform1i(2, 2); // Bind uniform 0 to texture 0
					
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, renderPassData.transparencyColorBucketBuffers[3]);
					glUniform1i(3, 3); // Bind uniform 0 to texture 0
				});
		}
		// Postprocessing Effects
		{
			// TODO: Actual Postprocessing
			int currentPostprocessingBuffer = 0;
			// Draw the final result to the screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glViewport(0,0, WIDTH,HEIGHT);
			
			glDisable(GL_BLEND);
			
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			drawQuad(*postProcessPassthrough, renderPassData.postProcessBuffers[currentPostprocessingBuffer]);
		}
		LogAvailableError();
		SDL_GL_SwapWindow(sdlWindow);
		return done;
	}

	void Context::drawQuad(Shader& shader, GLuint buffer){
		drawQuadAdvanced(shader, [buffer](){
				// Bind the current postprocessing buffer to tex0
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, buffer);
				glUniform1i(0, 0); // Bind uniform 0 to texture 0
			});
	}
	void Context::drawQuadAdvanced(Shader& shader, std::function<void()> bindTextures){
		glUseProgram(shader.programId);
		bindTextures();
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
