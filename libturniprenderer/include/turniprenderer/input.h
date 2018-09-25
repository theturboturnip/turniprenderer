#pragma once

#include "private/external/glm.h"

namespace TurnipRenderer {
	class Context;
	
	class Input {
	public:
		struct Button {
			enum class State {
				Off,
					Pressed,
					On,
					Released
					};
			inline State getState() const {
				return state;
			}
			inline bool isActive() const {
				return (state == State::Pressed) || (state == State::On);
			}

			inline void press(){
				if (state == State::Off
					|| state == State::Released){
					state = State::Pressed;
				}
			}
			inline void release(){
				if (state == State::On
					|| state == State::Pressed){
					state = State::Released;
				}
			}
			inline void setStateFromBool(bool pressed){
				if (pressed) press();
				else release();
			}
		private:
			friend class Input;
			void onFrameStart(){
				if (state == State::Pressed) state = State::On;
				else if (state == State::Released) state = State::Off;
			}

			State state = State::Off;
		};
		
		struct {
			struct {
				glm::vec2 deltaPos = glm::vec2(0);

				float scrollAmount = 0;
			} mouse;

		private:
			friend class Input;
			void reset(){
				mouse.deltaPos = glm::vec2(0);
				mouse.scrollAmount = 0;
			}
		} perFrame;
		
		struct {
			glm::vec2 pos;

			Button leftButton;
			Button rightButton;
		} mouse;

	private:
		friend class Context;
		void onFrameStart(){
			perFrame.reset();

			mouse.leftButton.onFrameStart();
			mouse.rightButton.onFrameStart();
		}
	};
};
