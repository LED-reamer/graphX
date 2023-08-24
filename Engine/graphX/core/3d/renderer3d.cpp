#include "renderer3d.h"

#include "graphX/internal/context.h"

void gx::renderer3d::lightPointSetParameters(gx::renderer3d::LightPoint* lightPoint, gx::Vec3 position, gx::Vec3 color, float intensity)
{
    lightPoint->position = position;
    lightPoint->ambient = color * 0.01f;
    lightPoint->diffuse = color * 0.5f;
    lightPoint->specular = color * 1.0f;

    lightPoint->constant = 0.468f * (1/intensity);
    lightPoint->linear = 0.141f * (1/intensity);
    lightPoint->quadratic = 0.0009f * (1/intensity);

    lightPoint->state = 1.0f;
}


gx::renderer3d::DeferredEnvironment* gx::renderer3d::getDeferredEnvironmentPtr()
{
	return &ctx3d.deferredEnvironment;
}

void gx::renderer3d::drawMesh(gx::Mesh* mesh, gx::Matrix* transform, gx::Material* material)
{
    ctx3d.drawCallQueueDeferred.emplace_back(gx::renderer3d::DrawCall{ mesh, transform == nullptr ? &ctx3d.defaultTransform : transform, material == nullptr ? &ctx3d.defaultMaterial : material });
}

void gx::renderer3d::drawModel(gx::Model* model, gx::Matrix* transform)
{
	for (size_t i = 0; i < model->submeshes.size(); i++)
	{
		drawMesh(&model->submeshes[i], transform, &model->materials[model->materialIds[i]]);
	}
}

gx::Texture* gx::renderer3d::getDeferredRenderTexturePtr()
{
    return &ctx3d.deferredOutput.textureAttachements[0];
}