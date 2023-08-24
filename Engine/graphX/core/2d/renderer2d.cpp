#include "renderer2d.h"

#include <iostream>

#include "graphX/vendor/GL/glew.h"

#include "graphX/internal/context.h"

#include "graphX/core/window.h"

#include "graphX/common/mathHelper.h"


void gx::renderer2d::setClearColor(gx::Vec3 col)
{
	glClearColor(col.r, col.g, col.b, 1);
}

void gx::renderer2d::clear(bool depthBufferBit)
{
	if(depthBufferBit)
		return glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gx::renderer2d::drawDot(gx::Vec2 pos, gx::Vec4 col, float r)
{
	if (ctx2d.batchCircleVertices.size() + 4 >= ctx2d.batchCircleBufferCapacity || ctx2d.batchCircleIndices.size() + 6 >= ctx2d.batchCircleBufferCapacity) ctx2d.flushBatchCircleRenderer();

	unsigned int offset = ctx2d.batchCircleVertices.size();


	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	float height = static_cast<float>(viewport[3]);


	ctx2d.batchCircleVertices.push_back(gx::CircleVertex2d{ {pos.x-r, pos.y+r}, col, r, {pos.x, height - pos.y} });
	ctx2d.batchCircleVertices.push_back(gx::CircleVertex2d{ {pos.x+r, pos.y+r}, col, r, {pos.x, height - pos.y} });
	ctx2d.batchCircleVertices.push_back(gx::CircleVertex2d{ {pos.x-r, pos.y-r}, col, r, {pos.x, height - pos.y} });
	ctx2d.batchCircleVertices.push_back(gx::CircleVertex2d{ {pos.x+r, pos.y-r}, col, r, {pos.x, height - pos.y} });

	ctx2d.batchCircleIndices.push_back(offset);
	ctx2d.batchCircleIndices.push_back(offset + 1);
	ctx2d.batchCircleIndices.push_back(offset + 2);
	ctx2d.batchCircleIndices.push_back(offset + 2);
	ctx2d.batchCircleIndices.push_back(offset + 1);
	ctx2d.batchCircleIndices.push_back(offset + 3);
}

void gx::renderer2d::drawLine(gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec4 col, float thickness)
{
	float d = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y));
	gx::Vec2 shift(-thickness * (pos2.y - pos1.y) / (d * 2.0f), thickness * (pos2.x - pos1.x) / (d * 2.0f));

	gx::renderer2d::drawRectangle(pos1 - shift, pos1 + shift, pos2 + shift, pos2 - shift, col);
}

void gx::renderer2d::drawLineList(std::vector<gx::Vec2> points, gx::Vec4 col, float thickness, bool fillCorners)
{
	drawLineList(&points[0], points.size(), col, thickness, fillCorners);
}

void gx::renderer2d::drawLineList(gx::Vec2* points, size_t size, gx::Vec4 col, float thickness, bool fillCorners)
{
	if (size < 2) return;//needs at least 2 points

	gx::Vec2 prevPoint1;
	gx::Vec2 prevPoint2;
	for (size_t i = 0; i < size; i++)
	{
		if (i == 0)//handle first two elements at once
		{
			gx::Vec2 pos1 = points[i];
			gx::Vec2 pos2 = points[i + 1];

			float d = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y));
			gx::Vec2 shift(-thickness * (pos2.y - pos1.y) / (d * 2.0f), thickness * (pos2.x - pos1.x) / (d * 2.0f));

			gx::renderer2d::drawRectangle(pos1 - shift, pos1 + shift, pos2 + shift, pos2 - shift, col);
			if (fillCorners)
			{
				prevPoint1 = pos2 - shift;
				prevPoint2 = pos2 + shift;
			}
			i++;
		}
		else
		{
			gx::Vec2 pos1 = points[i - 1];
			gx::Vec2 pos2 = points[i];

			float d = sqrt((pos2.x - pos1.x) * (pos2.x - pos1.x) + (pos2.y - pos1.y) * (pos2.y - pos1.y));
			gx::Vec2 shift(-thickness * (pos2.y - pos1.y) / (d * 2.0f), thickness * (pos2.x - pos1.x) / (d * 2.0f));
			gx::renderer2d::drawRectangle(pos1 - shift, pos1 + shift, pos2 + shift, pos2 - shift, col);

			if (fillCorners)
			{
				gx::renderer2d::drawRectangle(prevPoint1, prevPoint2, pos1 + shift, pos1 - shift, col);

				prevPoint1 = pos2 - shift;
				prevPoint2 = pos2 + shift;
			}
		}
	}
}

void gx::renderer2d::drawTriangle(gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec2 pos3, gx::Vec4 col)
{
	if (ctx2d.batchVertices.size() + 3 >= ctx2d.batchBufferCapacity || ctx2d.batchIndices.size() + 3 >= ctx2d.batchBufferCapacity) ctx2d.flushBatchRenderer();

	unsigned int offset = ctx2d.batchVertices.size();


	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos1, col });
	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos2, col });
	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos3, col });

	ctx2d.batchIndices.push_back(offset);
	ctx2d.batchIndices.push_back(offset + 1);
	ctx2d.batchIndices.push_back(offset + 2);
}

void gx::renderer2d::drawTriangleList(std::vector<gx::Vec2> points, gx::Vec4 col)
{
	drawTriangleList(&points[0], points.size(), col);
}

void gx::renderer2d::drawTriangleList(gx::Vec2* points, size_t size, gx::Vec4 col)
{
	for (size_t i = 0; i < size; i++)
	{
		if (i + 3 <= size)//only continue if enough points for a triangle
		{
			gx::renderer2d::drawTriangle(points[i], points[i + 1], points[i + 2], col);
			i += 2;
		}
		else
			return;
	}
}

void gx::renderer2d::drawRectangle(gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec2 pos3, gx::Vec2 pos4, gx::Vec4 col)
{
	if (ctx2d.batchVertices.size() + 4 >= ctx2d.batchBufferCapacity || ctx2d.batchIndices.size() + 6 >= ctx2d.batchBufferCapacity) ctx2d.flushBatchRenderer();

	unsigned int offset = ctx2d.batchVertices.size();


	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos1, col });
	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos2, col });
	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos3, col });
	ctx2d.batchVertices.push_back(gx::PrimitiveVertex2d{ pos4, col });
	
	ctx2d.batchIndices.push_back(offset);
	ctx2d.batchIndices.push_back(offset + 1);
	ctx2d.batchIndices.push_back(offset + 2);
	ctx2d.batchIndices.push_back(offset);
	ctx2d.batchIndices.push_back(offset + 2);
	ctx2d.batchIndices.push_back(offset + 3);
}

void gx::renderer2d::drawTexture(gx::Texture* texture, gx::Vec2 pos1, gx::Vec2 pos2, gx::Vec2 pos3, gx::Vec2 pos4, gx::Vec4 color, gx::Vec4 srcRect, float layer)
{
	if (ctx2d.batchTextureVertices.size() + 4 >= ctx2d.batchTextureBufferCapacity || ctx2d.batchTextureIndices.size() + 6 >= ctx2d.batchTextureBufferCapacity) ctx2d.flushBatchTextureRenderer();

	//texture caching
	auto found = std::find_if(ctx2d.batchTextureTextures.begin(), ctx2d.batchTextureTextures.end(), [&texture](const auto& pair)
	{
		return pair.second == texture;
	});
	
	float slot = 0;
	if (found != ctx2d.batchTextureTextures.end())
	{
		slot = found->first;
	}
	else
	{
		slot = (float)ctx2d.batchTextureTextures.size();
		ctx2d.batchTextureTextures.insert(std::make_pair(slot, texture));
	}

	if(srcRect.x == -1 || srcRect.y == -1 || srcRect.z == -1 || srcRect.w == -1)
	{
		srcRect.x = 0;
		srcRect.y = 0;
		srcRect.z = 1;
		srcRect.w = 1;
	}
	else
	{
		//converting pixel sizes to relative sizes
		srcRect.x /= texture->width;
		srcRect.y /= texture->height;
		srcRect.z /= texture->width;
		srcRect.w /= texture->height;
	}


	unsigned int offset = ctx2d.batchTextureVertices.size();

	ctx2d.batchTextureVertices.push_back(gx::TextureVertex3d{ {pos1, layer}, color, {0.0f, 1.0f}, slot, srcRect });
	ctx2d.batchTextureVertices.push_back(gx::TextureVertex3d{ {pos2, layer}, color, {1.0f, 1.0f}, slot, srcRect });
	ctx2d.batchTextureVertices.push_back(gx::TextureVertex3d{ {pos3, layer}, color, {0.0f, 0.0f}, slot, srcRect });
	ctx2d.batchTextureVertices.push_back(gx::TextureVertex3d{ {pos4, layer}, color, {1.0f, 0.0f}, slot, srcRect });
	
	ctx2d.batchTextureIndices.push_back(offset);
	ctx2d.batchTextureIndices.push_back(offset + 2);
	ctx2d.batchTextureIndices.push_back(offset + 1);
	ctx2d.batchTextureIndices.push_back(offset + 2);
	ctx2d.batchTextureIndices.push_back(offset + 3);
	ctx2d.batchTextureIndices.push_back(offset + 1);


	//prevent too many textures being bound at once
	if (ctx2d.batchTextureTextures.size() >= ctx2d.batchTextureSlots)
		ctx2d.flushBatchTextureRenderer();
}

void gx::renderer2d::drawTexture(gx::Texture* texture, gx::Vec2 pos, gx::Vec2 size, gx::Vec4 color, gx::Vec4 srcRect, float rotation, gx::Vec2 origin, float layer)
{
	gx::Vec2 pos2;
	gx::Vec2 pos3;
	gx::Vec2 pos4;

	if (size.x == -1)
		size.x = texture->width;
	if (size.y == -1)
		size.y = texture->height;
	
	pos2 = { pos.x + size.x, pos.y };
	pos3 = { pos.x, pos.y + size.y };
	pos4 = pos + size;

	if(rotation != 0)
	{
		pos = gx::mathHelper::rotatePoint(pos, origin, rotation);
		pos2 = gx::mathHelper::rotatePoint(pos2, origin, rotation);
		pos3 = gx::mathHelper::rotatePoint(pos3, origin, rotation);
		pos4 = gx::mathHelper::rotatePoint(pos4, origin, rotation);
	}
	
	gx::renderer2d::drawTexture(texture, pos, pos2, pos3, pos4, color, srcRect, layer);
}