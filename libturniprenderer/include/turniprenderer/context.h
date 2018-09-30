#pragma once

#include <string>
#include <functional>

#include "assets/asset_manager.h"
#include "debug_window.h"
#include "resource_manager.h"
#include "engine_fwd.h"
#include "scene.h"
#include "shader.h"
#include "input.h"
#include "material.h"
#include "texture.h"

#include "private/external/imgui.h"
#include <SDL.h>

namespace TurnipRenderer{
	class Context {
	public:
		std::string name = "Context!";
		ResourceManager<Mesh, Shader, Material, Texture> resources;
		Scene scene;

		struct CameraData {
			float fovDegrees;
			float depthMin;
			float depthMax;
			void updateProjectionMatrix();
			const inline glm::mat4& getTransformProjectionFromView(){
				return transformProjectionFromView;
			}
			
		private:
			glm::mat4 transformProjectionFromView = glm::mat4(1.0f);
		} cameraData;

		struct RenderPassData {
			union {
				struct {
					GLuint colorBuffer = 0;
					GLuint dummy;
				};
				GLuint postProcessBuffers[2]; // Post Processing is done 0->1->0->1...
			};
				
			GLuint opaqueDepthBuffer = 0;
			GLuint transparencyColorBucketBuffers[4] = {0, 0, 0, 0};
			GLuint transparencyDepthBuffer = 0;

			GLuint opaqueFramebuffer = 0;
			GLuint postProcessingFramebuffers[2] = {0, 0};
			GLuint transparencyBucketingFramebuffer = 0;

			// This is to make clang happy, the union means it deletes the default constructor
			RenderPassData() : colorBuffer(0), dummy(0) {}
		} renderPassData;

		Context(std::string name);
		~Context();
		bool renderFrame();
		void initDemoScene();
		void initWindow();

		inline const Input& getInput() const {
			return input;
		}
		inline const DefaultShaders& getDefaultShaders() const {
			return defaultShaders;
		}
		inline const DebugShaders& getDebugShaders() const {
			return debugShaders;
		}

		AssetManager assetManager;

		constexpr static size_t WIDTH = 1280;
		constexpr static size_t HEIGHT = 720;

		struct Shadowmap {
			GLuint colorBuffer;
			GLuint depthBuffer;
			glm::mat4 VP;
		};
		std::vector<Shadowmap> shadowmapsToUse;

		void LogAvailableError();
		
	private:

		Input input;
		
		SDL_Window* sdlWindow = nullptr;
		SDL_GLContext openGlContext;
		ImGuiIO* io = nullptr;

		Debug::DebugWindow debugWindow;
		
		DebugShaders debugShaders;
		DefaultShaders defaultShaders;

		ResourceHandle<Mesh> quad;
		ResourceHandle<Shader> debugTransparentProgram;
		ResourceHandle<Shader> postProcessPassthrough;
		ResourceHandle<Shader> transparencyResolve;

		void createFramebuffers();
		void drawMesh(Mesh& mesh);
		void drawQuad(Shader& shader, GLuint buffer);
		void drawQuadAdvanced(Shader& shader, std::function<void()> bindTextures);
	};
}
