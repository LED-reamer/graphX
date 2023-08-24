#pragma once
#include "graphX/core/types.h"

namespace gx
{
	namespace camera
	{
		Vec3* getPositionPtr();
		Vec3* getDirectionPtr();
		Vec3* getUpPtr();
		Matrix* getViewMatrixPtr();
	}
}