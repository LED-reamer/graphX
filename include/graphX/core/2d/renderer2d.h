#pragma once

#include <vector>

#include "graphX/vendor/glm/glm.hpp"
#include "graphX/core/types.h"

namespace gx
{
	namespace renderer2d
	{
		void setClearColor(glm::vec3 col);
		void clear();
		void drawDot(glm::vec2 pos, glm::vec4 col, float r = 10.0f);
		void drawLine(glm::vec2 pos1, glm::vec2 pos2, glm::vec4 col, float thickness = 1.0f);
		void drawLineList(std::vector<glm::vec2> points, glm::vec4 col, float thickness = 1.0f, bool fillCorners = true);
		void drawLineList(glm::vec2* points, size_t size, glm::vec4 col, float thickness = 1.0f, bool fillCorners = true);
		void drawTriangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec4 col);
		void drawTriangleList(std::vector<glm::vec2> points, glm::vec4 col);
		void drawTriangleList(glm::vec2* points, size_t size, glm::vec4 col);
		void drawRectangle(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 col);
		std::vector<glm::vec2> triangulatePolygon(std::vector<glm::vec2> points);
		std::vector<glm::vec2> triangulatePolygon(glm::vec2* points, size_t size);
		void drawTexture(gx::Texture* texture, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 pos3, glm::vec2 pos4, glm::vec4 color = { 1, 1, 1, 1 }, glm::vec4 srcRect = { -1, -1, -1, -1 });/*source rectangle*/
		void drawTexture(gx::Texture* texture, glm::vec2 pos, glm::vec2 size = { -1, -1 }, glm::vec4 color = { 1, 1, 1, 1 }, glm::vec4 srcRect = { -1, -1, -1, -1 }, float rotation = 0.0f, glm::vec2 origin = {0, 0});
	}
}