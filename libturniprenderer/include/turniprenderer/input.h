#pragma once

namespace TurnipRenderer {
	class Context;
	
	class Input {
	public:
		struct {
			struct {
				int deltaX = 0;
				int deltaY = 0;

				float scrollAmount = 0;
			} mouse;

		private:
			friend class Context;
			void reset(){
				mouse.deltaX = 0;
				mouse.deltaY = 0;
				mouse.scrollAmount = 0;
			}
		} perFrame;
		
		struct {
			int x = 0;
			int y = 0;
		} mouse;
	};
};
