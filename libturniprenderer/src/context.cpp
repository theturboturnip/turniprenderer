#include "turniprenderer/context.h"

#include <cstdio>

#include "turniprenderer/assets/asset_manager.h"
#include "turniprenderer/entity.h"
#include "turniprenderer/rendering/mesh.h"
#include "turniprenderer/rendering/renderhelpers.h"
#include "turniprenderer/system.h"
#include "turniprenderer/external/imgui.h"
#include "turniprenderer/dirlight_system.h"

namespace TurnipRenderer{
	Context::Context(std::string name) :
		name(std::move(name)),
		scene(*this),
		assetManager(*this),
		renderer(*this),
		debugWindow(*this),
		debugShaders(*this),
		defaultShaders(*this)
		{}
	
	void Context::initWindow(){
		renderer.initialize(
							name,
							glm::uvec2(WIDTH, HEIGHT),
							4,//OPENGL_MAJOR,
							6//OPENGL_MINOR
							);
		
		createFramebuffers();

		io = &ImGui::GetIO();
		ImGui::StyleColorsDark();

		debugShaders.createShaders();
		defaultShaders.createShaders();

		scene.systems.push_back(std::make_unique<DirectionalLightRenderer>(*this));
	}


	void Context::createFramebuffers(){
		auto createColorBuffer = [this](GLint internalFormat, GLenum format, GLenum type = GL_FLOAT){
			TextureConfig config = {
				glm::uvec2(WIDTH, HEIGHT),
				{ internalFormat, format, type },
				{ GL_NEAREST, GL_NEAREST },
				GL_CLAMP_TO_EDGE				
			};
			renderer.setOperation("Initial Colorbuffer Creation");
	
			return renderer.createColorBuffer(config);
		};
		auto createDepthBuffer = [this]() {
			TextureConfig config = {
				glm::uvec2(WIDTH, HEIGHT),
				{ GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_FLOAT },
				{ GL_NEAREST, GL_NEAREST },
				GL_CLAMP_TO_EDGE				
			};
			renderer.setOperation("Initial Depthbuffer Creation");
			return renderer.createDepthBuffer(config);
		};


		renderPassData.postProcessBuffers[0] = createColorBuffer(GL_RGB8, GL_RGB);
		renderPassData.postProcessBuffers[1] = createColorBuffer(GL_RGB8, GL_RGB);
		renderPassData.transparencyColorBucketBuffers[0] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.transparencyColorBucketBuffers[1] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.transparencyColorBucketBuffers[2] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.transparencyColorBucketBuffers[3] = createColorBuffer(GL_RGBA8, GL_RGBA);
		renderPassData.opaqueDepthBuffer = createDepthBuffer();
		renderPassData.transparencyDepthBuffer = renderPassData.opaqueDepthBuffer;//createDepthBuffer();

		renderer.setOperation("Initial Framebuffer Creation");

		renderPassData.opaqueFramebuffer = renderer.createFramebuffer(renderPassData.colorBuffer, renderPassData.opaqueDepthBuffer);
		renderPassData.postProcessingFramebuffers[0] = renderer.createFramebuffer(renderPassData.postProcessBuffers[0]);
		renderPassData.postProcessingFramebuffers[1] = renderer.createFramebuffer(renderPassData.postProcessBuffers[1]);
		renderPassData.transparencyBucketingFramebuffer = renderer.createFramebuffer(renderPassData.transparencyColorBucketBuffers, 4, renderPassData.transparencyDepthBuffer);
	}

	void Context::initDemoScene(){
		scene.camera = scene.addObjectToEndOfRoot("Camera", glm::vec3(0,0,10));

		cameraData.fovDegrees = 60;
		cameraData.depthMin = 0.1f;
		cameraData.depthMax = 500.f;
		cameraData.updateProjectionMatrix();

		debugTransparentProgram = resources.addResource(std::make_unique<UnlitShader>(R"(
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
		postProcessPassthrough = resources.addResource(std::make_unique<UnlitShader>(passthroughVertexShader,
															  R"(
layout(location = 0) uniform sampler2D tex;

layout(location = 0) in struct {
    vec2 uv0;
} IN;

layout(location = 0) out vec4 color;

void main(){
color = vec4(texture(tex, IN.uv0));
}
)"));

		transparencyResolve = resources.addResource(std::make_unique<UnlitShader>(passthroughVertexShader,
															  R"(
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

	void Context::setLightingShaderPath(std::string filePath){
		// TODO: This is bad. we should read the asset directly as a string
		// and let C++ do some char conversions if necessary (ex. newlines)
		std::vector<unsigned char> fileData = assetManager.readAsset(filePath);
		defaultShaders.setLightingCode(std::string(reinterpret_cast<char*>(fileData.data())));
	}

	void Context::CameraData::updateProjectionMatrix(){
		transformProjectionFromView = glm::perspective(glm::radians(fovDegrees), WIDTH/(1.0f * HEIGHT), depthMin, depthMax);
	}

	bool Context::renderFrame(){
		bool done = false;
		input.onFrameStart();
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);

			if (event.type == SDL_QUIT)
				done = true;
			else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(renderer.sdlWindow))
				done = true;

			if (!io->WantCaptureMouse){
				if (event.type == SDL_MOUSEMOTION){
					input.perFrame.mouse.deltaPos.x += static_cast<float>(event.motion.xrel);
					input.perFrame.mouse.deltaPos.y += static_cast<float>(event.motion.yrel);
					input.mouse.pos.x = event.motion.x;
					input.mouse.pos.y = event.motion.y;
				}else if (event.type == SDL_MOUSEWHEEL){
					input.perFrame.mouse.scrollAmount += -static_cast<float>(event.wheel.y);
					if (event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED)
						input.perFrame.mouse.scrollAmount = -input.perFrame.mouse.scrollAmount;
				}else if (event.type == SDL_MOUSEBUTTONDOWN){
					if (event.button.button == SDL_BUTTON_LEFT){
						input.mouse.leftButton.press();
					}else if (event.button.button == SDL_BUTTON_RIGHT){
						input.mouse.rightButton.press();
					}
				}else if (event.type == SDL_MOUSEBUTTONUP){
					if (event.button.button == SDL_BUTTON_LEFT){
						input.mouse.leftButton.release();
					}else if (event.button.button == SDL_BUTTON_RIGHT){
						input.mouse.rightButton.release();
					}
				}
			}
		}
		
		glCullFace(GL_BACK);

		shadowmapsToUse.clear();
		
		for(const auto& system : scene.systems){
			for (Entity* entity : scene.heirarchy){
				system->runOnEntityIfValid(entity);
			}
		}

		renderer.startFrame();
		
		glm::mat4 transformViewFromWorld = glm::inverse(scene.camera->transform.transformWorldSpaceFromModelSpace());
		glm::mat4 transformProjectionFromWorld = cameraData.getTransformProjectionFromView() * transformViewFromWorld;
		glm::vec3 cameraPos = scene.camera->transform.worldPosition();
		Shadowmap* shadowmap = nullptr;
		if (shadowmapsToUse.size() > 0) {
			shadowmap = &shadowmapsToUse[0];
		}
		RenderHelpers::StaticRenderData staticRenderData {transformProjectionFromWorld, shadowmap, cameraPos};

		// Depth Pass
		{
			renderer.setOperation("Opaque Depth Prepass");
			renderer.bindFrameBuffer(renderPassData.opaqueFramebuffer);
			
			glDisable(GL_BLEND);
			
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_TRUE);

			glClear(GL_DEPTH_BUFFER_BIT);

			glUseProgram(defaultShaders.depthOnlyShader->programId);
			for (auto* entity : scene.heirarchy){
				if (entity->renderable() && entity->material->isOpaque()){
					glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
					glUniformMatrix4fv(0, 1, GL_FALSE,
									   reinterpret_cast<const GLfloat*>(&MVP));

					renderer.drawMesh(*entity->mesh);
				}
			}
		}
		auto drawEntity = [&](Entity* entity){
			RenderHelpers::drawEntity(*this, *entity, staticRenderData);
		};
		{
			renderer.setOperation("Opaque Draw");
			renderer.bindFrameBuffer(renderPassData.opaqueFramebuffer);
			// Note: Don't clear to white here otherwise if there's nothing in the scene it will look like the program has crashed
			glClearColor(1,0.5,1,0);

			glDisable(GL_BLEND);
			
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_EQUAL);
			glDepthMask(GL_FALSE);
			
			glClear(GL_COLOR_BUFFER_BIT);

			for (auto* entity : scene.heirarchy){
				if (entity->renderable() && entity->material->isOpaque()){
					drawEntity(entity);
				}
			}
		}
		// Draw to transparency buffer
		{
			renderer.setOperation("Transparency Draw");
			//renderer.bindFrameBuffer(renderPassData.transparencyBucketingFramebuffer);
			//glClearColor(0,0,0,0);
			//glClear(GL_COLOR_BUFFER_BIT);
			//glUseProgram(debugTransparentProgram->programId);

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glDepthMask(GL_FALSE);
			
			//glUniform1f(1, cameraData.depthMin);
			//glUniform1f(2, cameraData.depthMax);

			for (auto* entity : scene.heirarchy){
				if (entity->renderable() && entity->material->isTranslucent() && entity->shader){
					/*glm::mat4 MVP = transformProjectionFromWorld * entity->transform.transformWorldSpaceFromModelSpace();
					glUniformMatrix4fv(0, 1, GL_FALSE,
									   reinterpret_cast<const GLfloat*>(&MVP));
					glUniform4fv(3, 1, reinterpret_cast<const GLfloat*>(&entity->material->color));
					renderer.drawMesh(*entity->mesh);*/
					drawEntity(entity);
				}
			}
		}
		// Blend transparency buffer onto opaque buffer
		/*{
			renderer.setOperation("Transparency Resolve");
			renderer.bindFrameBuffer(renderPassData.opaqueFramebuffer);
			
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			renderer.drawFullscreenQuadAdvanced(transparencyResolve.resourcePointer(), [this]() {
					renderer.bindTextureToSlot(GL_TEXTURE0, renderPassData.transparencyColorBucketBuffers[0]);
					glUniform1i(0, 0); // Bind uniform 0 to texture 0
					
					renderer.bindTextureToSlot(GL_TEXTURE1, renderPassData.transparencyColorBucketBuffers[1]);
					glUniform1i(1, 1); // Bind uniform 0 to texture 0
					
					renderer.bindTextureToSlot(GL_TEXTURE2, renderPassData.transparencyColorBucketBuffers[2]);
					glUniform1i(2, 2); // Bind uniform 0 to texture 0
					
					renderer.bindTextureToSlot(GL_TEXTURE3, renderPassData.transparencyColorBucketBuffers[3]);
					glUniform1i(3, 3); // Bind uniform 0 to texture 0
				});

			//fprintf(stderr, "Finished the transparency blend pass\n");
			}*/
		// Postprocessing Effects
		{
			renderer.setOperation("Post-Processing");
            // TODO: Actual Postprocessing
			int currentPostprocessingBuffer = 0;
			// Draw the final result to the screen
			renderer.bindWindowFramebuffer();
			
			glDisable(GL_BLEND);
			
			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			renderer.drawFullscreenQuad(postProcessPassthrough.resourcePointer(), renderPassData.postProcessBuffers[currentPostprocessingBuffer]);
		}

		// ImGui
		{
			renderer.setOperation("ImGui - Really shouldn't throw");
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplSDL2_NewFrame(renderer.sdlWindow);
			ImGui::NewFrame();
			
			//ImGui::ShowDemoWindow(nullptr);
			debugWindow.show();

			if (ImGui::Begin("Input", nullptr)){
				ImGui::Text("Mouse Pos: %f %f", input.mouse.pos.x, input.mouse.pos.y);
				ImGui::Text("LMB: %d", input.mouse.leftButton.getState());
				ImGui::Text("RMB: %d", input.mouse.rightButton.getState());
				ImGui::Text("Per-Frame");
				{
					ImGui::Indent();
					ImGui::Text("Delta Pos: %f %f", input.perFrame.mouse.deltaPos.x, input.perFrame.mouse.deltaPos.y);
					ImGui::Text("Delta Scroll: %f", input.perFrame.mouse.scrollAmount);
					ImGui::Unindent();
				}
			}
			ImGui::End();
			
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		}
		renderer.endFrame();
		return done;
	}
};
