#include "camera.h"

#include "graphX/internal/context.h"

gx::Vec3* gx::camera::getPositionPtr()
{
	return &ctx.cameraPosition;
}

gx::Vec3* gx::camera::getDirectionPtr()
{
	return &ctx.cameraDirection;
}

gx::Vec3* gx::camera::getUpPtr()
{
	return &ctx.cameraUp;
}

gx::Matrix* gx::camera::getViewMatrixPtr()
{
	ctx.cameraViewMatrix = gx::matrixLookAt(ctx.cameraPosition, ctx.cameraPosition + ctx.cameraDirection, ctx.cameraUp);
	return &ctx.cameraViewMatrix;
}