#include "texture.h"

#include "stb/stb_image.h"

#include <cstdio>
#include <assert.h>

#include <chrono>

namespace TurnipRenderer {
	Texture::Texture(std::vector<unsigned char>&& assetData){
		unsigned char* dataPtr = stbi_load_from_memory(assetData.data(), assetData.size(), &width, &height, &channels, 0);
		assert(dataPtr);
		data = std::vector<unsigned char>(dataPtr, dataPtr + (width * height * channels));
		stbi_image_free(dataPtr);

		{
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
		}

		GLenum imageFormat = GL_RED;
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
		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, width, height, 0, imageFormat, GL_UNSIGNED_BYTE, data.data());
		glGenerateMipmap(GL_TEXTURE_2D);

		{
			GLuint glError = glGetError();
			if (glError){
				fprintf(stderr, "Texture OpenGL Error: %d\n", glError);
				assert(false);
			}
		}
	}
};
