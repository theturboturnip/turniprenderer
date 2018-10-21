#pragma once

#include "turniprenderer/resource.h"
#include "turniprenderer/rendering/texture.h"

namespace TurnipRenderer {
	class Material {
	public:
		enum class TransparencyMode {
			Opaque,
				Translucent,
				AlphaClipped
				};
		
		Material(ResourceHandle<Texture> texture, TransparencyMode transparencyMode, glm::vec4 color)
			: texture(texture), transparencyMode(transparencyMode), color(color) {}
		
		ResourceHandle<Texture> texture;
		
		TransparencyMode transparencyMode;
		glm::vec4 color;

		// TODO: Handle alphaClipped
		inline bool isOpaque() const {
			return transparencyMode == TransparencyMode::Opaque;
		}
		inline bool isTranslucent() const {
			return transparencyMode == TransparencyMode::Translucent;
		}
	};
};
