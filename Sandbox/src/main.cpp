#include "graphX/graphX.h"


//next todos:
// 
// resize framebuffers only when resizing window!!
// 
// renderer3d with drawcall mangegement and efficent instancing, deferred rendering and forward rendering for transparent
// texture loading more efficent. Maybe threads and caching
// model loading from binary
// model loading proper threading support
// mdsf text rendering
// 
// shader/object bound system -> bind if not already, else do nothing per function
//

int main()
{
	gx::debug::test(gx::init("EngineTest", 1920, 1080, -1, -1, true), "could not init the engine!", true);
	gx::renderer2d::setClearColor({ 1.0f, 1.0f, 1.0f });


	gx::Model m = gx::modelLoadFromFile("res/models/untitled2.fbx");

	gx::renderer3d::getDeferredEnvironmentPtr()->pointLights.push_back(gx::renderer3d::LightPoint());
	gx::renderer3d::getDeferredEnvironmentPtr()->pointLights.back().state = 1;
	gx::renderer3d::lightPointSetParameters(&gx::renderer3d::getDeferredEnvironmentPtr()->pointLights.back(), { -5, 2, 0 }, {1, .8f, .6f}, 3);



	while (gx::running())
	{

		{
			if (gx::input::keyDown(gx::input::W))
				gx::camera::getPositionPtr()->z += 1.5f;
			if (gx::input::keyDown(gx::input::S))
				gx::camera::getPositionPtr()->z -= 1.5f;

			if (gx::input::keyDown(gx::input::D))
				gx::camera::getPositionPtr()->x -= 1.5f;
			if (gx::input::keyDown(gx::input::A))
				gx::camera::getPositionPtr()->x += 1.5f;

			if (gx::input::keyDown(gx::input::SPACE))
				gx::camera::getPositionPtr()->y += 1.5f;
			if (gx::input::keyDown(gx::input::LEFT_SHIFT))
				gx::camera::getPositionPtr()->y -= 1.5f;
		}


		gx::Matrix mat = gx::matrixCreate();
		mat = glm::rotate(mat, glm::radians(90.0f), { -1, 0, 0 });
		gx::renderer3d::drawModel(&m, &mat);

		gx::renderer2d::drawTexture(gx::renderer3d::getDeferredRenderTexturePtr());

		{
			ImGui::Begin("DEBUG");
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0 / double(ImGui::GetIO().Framerate), double(ImGui::GetIO().Framerate));
			ImGui::End();
		}
	}

	gx::modelDelete(&m);

	gx::terminate();


	//71 MB
	return 0;
}