#include "turniprenderer/texture.h"

#include "turniprenderer/context.h"

#include <cstdio>
#include <assert.h>

#include <chrono>

namespace TurnipRenderer {
	Texture::Texture(Context& context, std::vector<unsigned char>&& data, int width, int height, int channels/*std::vector<unsigned char>&& assetData*/)
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

		/*{
			GLuint glError = glGetError();
			if (glError){
				fprintf(stderr, "Texture OpenGL Error Pre-Create: %d\n", glError);
				assert(false);
			}
		}
		
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		{
			GLuint glError = glGetError();
			if (glError){
				fprintf(stderr, "Texture OpenGL Error Pre-Set-Trilinear: %d\n", glError);
				assert(false);
			}
		}

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Trilinear Filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		{
			GLuint glError = glGetError();
			if (glError){
				fprintf(stderr, "Texture OpenGL Error Pre-Format: %d\n", glError);
				assert(false);
				}
				}*/

		/*GLenum imageFormat = GL_RED;
		switch(channels){
		case 1:
			imageFormat = GL_RED;
			break;
		case 2:
			imageFormat = GL_RG;
			break;
		case 3:
			imageFormat = GL_RGB;
			break;
		case 4:
			imageFormat = GL_RGBA;
			break;
		default:
			assert(false);
		}
		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data.data());*/
		//glGenerateMipmap(GL_TEXTURE_2D);
		//bindlessTextureId = glGetTextureHandleARB(textureId);

		/*{
			GLuint glError = glGetError();
			if (glError){
				fprintf(stderr, "Texture OpenGL Error: %d\n", glError);
				assert(false);
			}
		}*/
	}
};
