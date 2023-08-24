#pragma once

#include "graphX/core/types.h"

namespace gx
{
	namespace renderer3d
	{
		//status to on (1 or not 0) by default
		struct LightDirectional
		{
			LightDirectional()
			{
				direction = { -0.2f, -0.8f, -0.3f };

				ambient = { 0.3f, 0.3f, 0.3f };
				diffuse = { 0.9f, 0.8f, 0.6f };
				specular = { 0.5f, 0.5f, 0.4f };

				state = 1;
			}

			gx::Vec3 direction;

			gx::Vec3 ambient;
			gx::Vec3 diffuse;
			gx::Vec3 specular;

			int state;
		};

		//status to off (0) by default
		struct LightPoint
		{
			LightPoint()
			{
				position = { 0.0f, 0.0f, 0.0f };

				constant = 0.468f;
				linear = 0.141f;
				quadratic = 0.001f;

				ambient = { 0.2f, 0.2f, 0.2f };
				diffuse = { 0.5f, 0.5f, 0.5f };
				specular = { 1.0f, 1.0f, 1.0f };

				state = 0;
			}

			gx::Vec3 position;

			float constant;
			float linear;
			float quadratic;

			gx::Vec3 ambient;
			gx::Vec3 diffuse;
			gx::Vec3 specular;

			int state;
		};
		void lightPointSetParameters(LightPoint* lightPoint, gx::Vec3 position, gx::Vec3 color, float power);

		//status to off (0) by default
		struct LightSpot
		{
			LightSpot()
			{
				position = { 0, 0, 0 };
				direction = { 0, -1, 0 };
				cutOff = glm::cos(glm::radians(12.5f));
				outerCutOff = glm::cos(glm::radians(17.5f));

				constant = 1.0f;
				linear = 0.09f;
				quadratic = 0.032f;

				ambient = { 0.0f, 0.0f, 0.0f };
				diffuse = { 1.0f, 1.0f, 1.0f };
				specular = { 1.0f, 1.0f, 1.0f };

				state = 0;
			}

			gx::Vec3 position;
			gx::Vec3 direction;
			float cutOff;
			float outerCutOff;

			float constant;
			float linear;
			float quadratic;

			gx::Vec3 ambient;
			gx::Vec3 diffuse;
			gx::Vec3 specular;

			int state;
		};

		struct DeferredEnvironment
		{
			gx::CubeMap skybox;

			//lights
			LightDirectional directionalLight;
			std::vector<LightPoint> pointLights;
			// TODO later std::vector<LightSpot> spotLight;
		};

		DeferredEnvironment* getDeferredEnvironmentPtr();

		//deferred rendering
		void drawMesh(gx::Mesh* mesh, gx::Matrix* transform = nullptr, gx::Material* material = nullptr);//important: check if pointer already in list -> instanced rendering !!materials and transforms individual
		//deferred rendering
		void drawModel(gx::Model* model, gx::Matrix* transform = nullptr);

		//deferred rendering
		//void drawMeshInstanced(gx::Mesh* mesh, gx::Material* material = nullptr);//important: check if pointer already in list -> instanced rendering !!materials and transforms individual
		//deferred rendering
		//void drawModelInstanced(gx::Model* model);

		Texture* getDeferredRenderTexturePtr();
	}
}