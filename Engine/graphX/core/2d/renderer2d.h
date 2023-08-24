#pragma once

#include <vector>

#include "graphX/vendor/glm/glm.hpp"
#include "graphX/core/types.h"

namespace gx
{
	//all drawing related functions draw CounterClockWise (CCW)
	namespace renderer2d
	{
		void setClearColor(gx::Vec3 col);
		void clear(bool depthBufferBit = false);
		void drawDot(gx::Vec2 pos, gx::Vec4 col, float r = 10.0f);
		void drawLine(gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec4 col, float thickness = 1.0f);
		void drawLineList(std::vector<gx::Vec2> points, gx::Vec4 col, float thickness = 1.0f, bool fillCorners = true);
		void drawLineList(gx::Vec2* points, size_t size, gx::Vec4 col, float thickness = 1.0f, bool fillCorners = true);
		void drawTriangle(gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec2 pos3, gx::Vec4 col);
		void drawTriangleList(std::vector<gx::Vec2> points, gx::Vec4 col);
		void drawTriangleList(gx::Vec2* points, size_t size, gx::Vec4 col);
		void drawRectangle(gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec2 pos3, gx::Vec2 pos4, gx::Vec4 col);
		void drawTexture(gx::Texture* texture, gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec2 pos3, gx::Vec2 pos4, gx::Vec4 color = { 1, 1, 1, 1 }, gx::Vec4 srcRect = { -1, -1, -1, -1 }, float layer = 0.0f);/*source rectangle*/
		void drawTexture(gx::Texture* texture, gx::Vec2 pos = { 0, 0 }, gx::Vec2 size = { -1, -1 }, gx::Vec4 color = { 1, 1, 1, 1 }, gx::Vec4 srcRect = { -1, -1, -1, -1 }, float rotation = 0.0f, gx::Vec2 origin = { 0, 0 }, float layer = 0.0f);
	}
}