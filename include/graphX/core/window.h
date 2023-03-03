#pragma once
#include <string>

//#include <glm/glm.hpp>
#include "../vendor/glm/glm.hpp"

namespace gx
{
	namespace window
	{
		void setSizeLimits(int _minWidth = 1, int _minHeight = 1, int _maxWidth = -1, int _maxHeight = -1);
		void setIcon(std::string path);
		void setTitle(std::string title);
		void setFpsCap(bool capped);
		void disableCursor(bool disabled);
		void setSize(unsigned int width, unsigned int height);
		void setPosition(int x, int y);
		glm::vec2 getSize();
		glm::vec2 getPosition();
	}
}