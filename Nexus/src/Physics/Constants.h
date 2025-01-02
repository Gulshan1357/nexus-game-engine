#pragma once

#include "App/AppSettings.h"

namespace Physics
{
	constexpr int PIXEL_PER_METER = 50; // Let's say one meter is represented by 50 pixels
	constexpr int SCREEN_WIDTH = APP_VIRTUAL_WIDTH; // 1024 : effective x resolution regardless of actual screen/window res
	constexpr int SCREEN_HEIGHT = APP_VIRTUAL_HEIGHT; // 768 : effective Y resolution regardless of actual screen/window res

	constexpr float gravity = 9.8f;
}