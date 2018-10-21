#pragma once

#include "turniprenderer/external/gl.h"
#include "turniprenderer/external/glm.h"

namespace TurnipRenderer {
		struct TextureConfig {
		glm::uvec2 size;

		struct Format {
			GLint internalFormat;
			GLenum format;
			GLenum dataType;
		} formatInfo;

		struct Filter {
			GLenum min = GL_LINEAR;
			GLenum mag = GL_LINEAR;
		} filtering;

		struct AddressMode {
			AddressMode(GLenum wrapMode)
				: wrapMode(wrapMode), borderColor(0) {}
			AddressMode(glm::vec4 borderColor)
				: wrapMode(GL_CLAMP_TO_BORDER), borderColor(borderColor) {}
			
			GLenum wrapMode; // Same for X,Y,Z
			glm::vec4 borderColor;
		} addressMode;
					 
		struct Compare {
			constexpr Compare() {};
			Compare(GLenum compareMode, GLenum compareFunc)
				: compareMode(compareMode), compareFunc(compareFunc) {}
			GLenum compareMode = GL_NONE;
			GLenum compareFunc = GL_LEQUAL;
		} compare;

		TextureConfig(glm::uvec2 size, Format formatInfo, Filter filtering, GLenum wrapMode, Compare compare = Compare())
			: TextureConfig(size, formatInfo, filtering, AddressMode(wrapMode), compare) {}
		TextureConfig(glm::uvec2 size, Format formatInfo, Filter filtering, glm::vec4 borderColor, Compare compare = Compare())
			: TextureConfig(size, formatInfo, filtering, AddressMode(borderColor), compare) {}
		TextureConfig(glm::uvec2 size, Format formatInfo, Filter filtering, AddressMode addressMode, Compare compare = Compare())
			: size(size),
			  formatInfo(formatInfo),
			  filtering(filtering),
			  addressMode(addressMode),
			  compare(compare) {}
	};
}
