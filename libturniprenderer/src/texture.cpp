#include "texture.h"

#include "stb/stb_image.h"

#include <cstdio>
#include <assert.h>

namespace TurnipRenderer {
	Texture::Texture(std::vector<unsigned char>&& assetData){
		unsigned char* dataPtr = stbi_load_from_memory(assetData.data(), assetData.size(), &width, &height, &channels, 0);
		fprintf(stdout, "assetData.data() = %p, assetData.size() = %zu, dataPtr = %p\n", assetData.data(), assetData.size(), dataPtr);
		assert(dataPtr);
		data = std::vector<unsigned char>(dataPtr, dataPtr + (width * height * channels));
		stbi_image_free(dataPtr);
		
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

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
	}
};
