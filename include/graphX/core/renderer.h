#pragma once

#include <vector>

//#include "glm/glm.hpp"
#include "../vendor/glm/glm.hpp"

namespace gx
{
	namespace renderer
	{
		void setClearColor(glm::vec3 col);
		void clear();
		void drawDot(glm::vec2 pos, glm::vec4 col, float r = 10.0f);
		void drawLine(glm::vec2 pos1, glm::vec2 pos2, glm::vec4 col, float thickness = 1.0f);
		void drawLineList(std::vector<glm::vec2> points, glm::vec4 col, float thickness = 1.0f, bool fillCorners = true);
		void drawTriangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec4 col);
		void drawTriangleList(std::vector<glm::vec2> points, glm::vec4 col);
		void drawRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 col);
		std::vector<glm::vec2> triangulatePolygon(std::vector<glm::vec2> points);

		//imgui
		void beginImGuiRendering();
		void endImGuiRendering();
	}
}