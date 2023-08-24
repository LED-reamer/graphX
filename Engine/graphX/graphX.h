#pragma once

#include <string>
#include "graphX/vendor/openglIncludes.h"
#include "graphX/vendor/glm/glm.hpp"
#include "graphX/vendor/glm/gtc/matrix_transform.hpp"
#include "graphX/vendor/stb_image/stb_image.h"
#include "graphX/vendor/imgui/imgui.h"

#include "graphX/core/types.h"
#include "graphX/core/window.h"
#include "graphX/core/camera.h"
#include "graphX/core/2d/renderer2d.h"
#include "graphX/core/3d/renderer3d.h"
#include "graphX/core/input.h"
#include "graphX/core/debug.h"
#include "graphX/core/memoryPackaging.h"

#include "graphX/common/common.h"

namespace gx
{
	bool init(std::string windowName = "", unsigned int windowWidth = 1, unsigned int windowHeight = 1, int windowMinWidth = -1, int windowMinHeight = -1, bool debugMode = false);
	bool running();
	bool terminate();
}