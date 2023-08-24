#include "window.h"

#include <iostream>

#include "graphX/vendor/openglIncludes.h"
#include "graphX/vendor/stb_image/stb_image.h"

#include "graphX/internal/context.h"


void gx::window::setSizeLimits(int _minWidth, int _minHeight, int _maxWidth, int _maxHeight)
{
	glfwSetWindowSizeLimits(ctx.window, _minWidth, _minHeight, _maxWidth, _maxHeight);
}

void gx::window::setIcon(std::string path)
{
	int width, height, channel;
	unsigned char* img = stbi_load(path.c_str(), &width, &height, &channel, 4); //rgba channels 
	if (img == NULL) {
		std::string errormsg = "Icon at \"" + std::string(path) + "\" could not be loaded !";
		std::cout << errormsg.c_str();
		return;
	}
	GLFWimage image;
	image.width = width;
	image.height = height;
	image.pixels = img;
	glfwSetWindowIcon(ctx.window, 1, &image);
	stbi_image_free(img);
}

void gx::window::setTitle(std::string title)
{
	glfwSetWindowTitle(ctx.window, title.c_str());
}

void gx::window::setFpsCap(bool capped)
{
	glfwSwapInterval(capped);//sync with monitor refresh rate,           1 = fps capped to 60fps, 0 = no fps cap
}

void gx::window::disableCursor(bool disabled)
{
	glfwSetInputMode(ctx.window, GLFW_CURSOR, disabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void gx::window::setSize(unsigned int width, unsigned int height)
{
	glfwSetWindowSize(ctx.window, width, height);
}

void gx::window::setPosition(int x, int y)
{
	glfwSetWindowPos(ctx.window, x, y);
}

gx::Vec2 gx::window::getSize()
{
	int w, h = 0;
	glfwGetWindowSize(ctx.window, &w, &h);
	return gx::Vec2(w, h);
}

gx::Vec2 gx::window::getPosition()
{
	int x, y = 0;
	glfwGetWindowPos(ctx.window, &x, &y);
	return gx::Vec2(x, y);
}