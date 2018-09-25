#pragma once

#include "turniprenderer/component.h"

class FreeCameraComponent : public TurnipRenderer::Component {
public:
	float mouseSensitivity = 0.01f;
	bool flipLookY = false;
};
