#include <graphX/graphX.h>

int main()
{
	gx::init("MyApp", 1920, 1080);

	while(gx::running())
	{
		gx::renderer2d::drawDot(gx::input::getMousePosition(), { 1, 1, 1, 1 }, 100.0f);
	}

	gx::terminate();
	return 0;
}