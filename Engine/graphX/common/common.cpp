#include "common.h"

#include "graphX/core/window.h"

#include "graphX/internal/triangulate.h"
#include "graphX/internal/context.h"

#include "graphX/vendor/glm/ext.hpp"

std::vector<glm::vec2> gx::common::triangulatePolygon(std::vector<glm::vec2> points)
{
	return triangulatePolygon(&points[0], points.size());
}

std::vector<glm::vec2> gx::common::triangulatePolygon(glm::vec2* points, size_t size)
{
	std::vector<glm::vec2> result;
	Triangulate::Process(points, size, result);

	return result;
}

gx::Matrix gx::common::getCurrentProjectionMatrixOrthographic()
{
	if (ctx.currentRenderTargets.back() == nullptr)
		return glm::ortho(0.0f, gx::window::getSize().x, gx::window::getSize().y, 0.0f);
	return glm::ortho(0.0f, (float)ctx.currentRenderTargets.back()->texturePtr->width, (float)ctx.currentRenderTargets.back()->texturePtr->height, 0.0f);
}

gx::Matrix gx::common::getCurrentProjectionMatrixPerspective(float fov, float zNear, float zFar)
{
	if (ctx.currentRenderTargets.back() == nullptr)
		return glm::perspective(fov, (float)gx::window::getSize().x / (gx::window::getSize().y == 0 ? 1 : gx::window::getSize().y), zNear, zFar);
	return glm::perspective(fov, (float)ctx.currentRenderTargets.back()->texturePtr->width / (float)(ctx.currentRenderTargets.back()->texturePtr->height == 0 ? 1 : (float)ctx.currentRenderTargets.back()->texturePtr->height), zNear, zFar);
}