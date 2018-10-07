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
#include "renderer.h"

#include "private/external/imgui.h"
#include <SDL.h>

namespace TurnipRenderer{
	class Context {
	public:
		std::string name = "TurnipRenderer Demo";
		ResourceManager<Mesh, UnlitShader, Shader, Material, Texture> resources;
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
			ResourceHandle<const ColorBuffer>& colorBuffer;
			ResourceHandle<const ColorBuffer> postProcessBuffers[2]; // Post Processing is done 0->1->0->1...
				
			ResourceHandle<const DepthBuffer> opaqueDepthBuffer = nullptr;
			ResourceHandle<const ColorBuffer> transparencyColorBucketBuffers[4] = {nullptr};
			ResourceHandle<const DepthBuffer> transparencyDepthBuffer = nullptr;

			ResourceHandle<const FrameBuffer> opaqueFramebuffer = nullptr;
			ResourceHandle<const FrameBuffer> postProcessingFramebuffers[2] = {nullptr};
			ResourceHandle<const FrameBuffer> transparencyBucketingFramebuffer = nullptr;

			// This is to make clang happy, the union means it deletes the default constructor
			RenderPassData() : colorBuffer(postProcessBuffers[0]) {}
		} renderPassData;

		Context(std::string name);
		bool renderFrame();
		void initDemoScene();
		void initWindow();

		void setLightingShaderPath(std::string filePath);

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

		constexpr static size_t OPENGL_MAJOR = 3;
		constexpr static size_t OPENGL_MINOR = 3;

		struct Shadowmap {
			ResourceHandle<const ColorBuffer> colorBuffer;
			ResourceHandle<const DepthBuffer> depthBuffer;
			glm::mat4 VP;
		};
		std::vector<Shadowmap> shadowmapsToUse;

		// TODO: Figure out a way to let Systems request renderer access
		// Then this should be made private
		Renderer renderer;
		
	private:

		Input input;
		
		ImGuiIO* io = nullptr;

		Debug::DebugWindow debugWindow;
		
		DebugShaders debugShaders;
		DefaultShaders defaultShaders;

		ResourceHandle<UnlitShader> debugTransparentProgram;
		ResourceHandle<UnlitShader> postProcessPassthrough;
		ResourceHandle<UnlitShader> transparencyResolve;

		void createFramebuffers();
	};
}
