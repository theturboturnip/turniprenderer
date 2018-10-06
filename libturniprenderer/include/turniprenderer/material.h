#pragma once

#include "resource.h"
#include "texture.h"

namespace TurnipRenderer {
	class Material {
	public:
		ResourceHandle<Texture> texture;
		
		enum class TransparencyMode {
			Opaque,
				Translucent,
				AlphaClipped
				};
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
