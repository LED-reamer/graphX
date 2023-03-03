#pragma once

#include <string>

//#include "openglInclude.h"
//#include <glm/glm.hpp>
//#include <stb_image/stb_image.h>
//#include <imgui/imgui.h>

#include "vendor/openglInclude.h"
#include "vendor/glm/glm.hpp"
#include "vendor/stb_image/stb_image.h"
#include "vendor/imgui/imgui.h"

#include "core/window.h"
#include "core/renderer.h"
#include "core/input.h"

namespace gx
{
	bool init(std::string windowName = "", unsigned int windowWidth = 1, unsigned int windowHeight = 1, int windowMinWidth = -1, int windowMinHeight = -1);
	bool running();
	bool terminate();
}