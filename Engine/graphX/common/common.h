#pragma once
#include "graphX/common/mathHelper.h"
#include "graphX/core/debug.h"
#include "graphX/core/types.h"

//general
#include <fstream>
#include <vector>

namespace gx
{
    namespace common
    {
        std::vector<glm::vec2> triangulatePolygon(std::vector<glm::vec2> points);
        std::vector<glm::vec2> triangulatePolygon(glm::vec2* points, size_t size);
        gx::Matrix getCurrentProjectionMatrixOrthographic();
        gx::Matrix getCurrentProjectionMatrixPerspective(float fov = 45.0f, float zNear = 0.1f, float zFar = 10000.0f);
    }
}
