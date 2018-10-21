#pragma once

#include "turniprenderer/external/gl.h"
#include "turniprenderer/external/glm.h"

#include "turniprenderer/resource.h"

#include "turniprenderer/rendering/texture_config.h"

#include <vector>

namespace TurnipRenderer {
	// TODO: Destructor? Figure how to do that
	struct AddressableBuffer {
		AddressableBuffer(GLuint handle, TextureConfig config)
			: config(config), handle(handle), bindlessHandle(0)  {}
		TextureConfig config;

	private:
		friend class Renderer;

		GLuint handle;
		GLuint64 bindlessHandle;
		// TODO: This will be weird if someone tries to use this with a bindless handle?
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
}
