#include "turniprenderer/rendering/texture.h"

#include "turniprenderer/context.h"

#include <cstdio>
#include <assert.h>

#include <chrono>

namespace TurnipRenderer {
	Texture::Texture(Context& context, std::vector<unsigned char>&& data, int width, int height, int channels)
		: ContextAware(context), data(data) {

		GLint internalFormat = GL_R8;
		GLenum imageFormat = GL_RED;
		switch(channels){
		case 1:
			imageFormat = GL_RED;
			internalFormat = GL_R8;
			break;
		case 2:
			imageFormat = GL_RG;
			internalFormat = GL_RG8;
			break;
		case 3:
			imageFormat = GL_RGB;
			internalFormat = GL_RGB8;
			break;
		case 4:
			imageFormat = GL_RGBA;
			internalFormat = GL_RGBA8;
			break;
		default:
			assert(false);
		}
		
		TextureConfig config(
			{width, height},
			{
				internalFormat,
					imageFormat,
					GL_UNSIGNED_BYTE
					},
			{ GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR },
			GL_REPEAT
			);

		buffer = context.renderer.createColorBuffer(config);
		context.renderer.fillColorBuffer(buffer, this->data);
		context.renderer.generateMipmapsForColorBuffer(buffer);
	}
};
