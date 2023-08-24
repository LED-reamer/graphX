#include "input.h"

#include "graphX/vendor/GLFW/glfw3.h"
#include "graphX/vendor/imgui/imgui.h"

#include "graphX/internal/context.h"

bool gx::input::keyDown(Key key)
{
	if (glfwGetKey(ctx.window, (int)key) == GLFW_PRESS)
		return true;
	return false;
}

gx::Vec2 gx::input::getMousePosition()
{
	double x, y;
	glfwGetCursorPos(ctx.window, &x, &y);
	return gx::Vec2{(float)x, (float)y};
}

bool gx::input::mouseButtonDown(MouseButton mouseButton)
{
	int state = glfwGetMouseButton(ctx.window, (int)mouseButton);
	if (state == GLFW_PRESS)
		return true;
	return false;
}

float gx::input::getMouseScrollOffset()
{
	return ImGui::GetIO().MouseWheel;
}