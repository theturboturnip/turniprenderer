#pragma once

#include "private/external/glm.h"

namespace TurnipRenderer {
	class Context;
	
	class Input {
	public:
		struct {
			struct {
				glm::vec2 deltaPos = glm::vec2(0);

				float scrollAmount = 0;
			} mouse;

		private:
			friend class Context;
			void reset(){
				mouse.deltaPos = glm::vec2(0);
				mouse.scrollAmount = 0;
			}
		} perFrame;
		
		struct {
			glm::vec2 pos;
		} mouse;
	};
};
