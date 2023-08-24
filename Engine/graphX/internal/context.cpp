#include "context.h"

#include <iostream>

#include "graphX/vendor/GL/glew.h"
#include "graphX/vendor/glm/gtc/matrix_transform.hpp"
#include "graphX/vendor/imgui/imgui.h"
#include "graphX/vendor/imgui/imgui_impl_glfw_gl3.h"

#include "graphX/core/window.h"
#include "graphX/core/camera.h"
#include "graphX/core/input.h"
#include "graphX/core/debug.h"
#include "graphX/core/memoryPackaging.h"

#include <graphX/common/common.h>

//#define LOAD_ASSETS//TODO comment out later


context ctx;

void context::init()
{
	quitting = false;
	debugMode = false;
	window = nullptr;

	currentRenderProfile = std::make_unique<gx::RenderProfile>(gx::renderProfileCreate());
	currentRenderTargets = {};
	currentRenderTargets.push_back(nullptr);

	audioDevice = nullptr;
	audioContext = nullptr;

	cameraPosition = {0, 0, 0};
	cameraDirection = {0, 0, 1};
	cameraUp = {0, 1, 0};
	cameraViewMatrix = gx::matrixCreate();
}

void context::loadResources()
{
#ifdef LOAD_ASSETS
	saveResourcesFromFiles();
	loadResourcesFromResourceStorage();
#else
	loadResourcesFromResourceStorage();
#endif
}

void context::saveResourcesFromFiles()
{
	//shaders
	gx::mem::addPackageShader(&ctx.internalResourceStorage, "shaders/primitive.shader", "res/shaders/primitive.shader");
	gx::mem::addPackageShader(&ctx.internalResourceStorage, "shaders/circle.shader", "res/shaders/circle.shader");
	gx::mem::addPackageShader(&ctx.internalResourceStorage, "shaders/texture.shader", "res/shaders/texture.shader");
	gx::mem::addPackageShader(&ctx.internalResourceStorage, "shaders/gBuffer.shader", "res/shaders/gBuffer.shader");
	gx::mem::addPackageShader(&ctx.internalResourceStorage, "shaders/deferred.shader", "res/shaders/deferred.shader");

	//default material
	ctx3d.defaultMaterial.name = "default";
	ctx3d.defaultMaterial.color = { 1, 1, 1 };
	ctx3d.defaultMaterial.metallic = 1.0f;
	ctx3d.defaultMaterial.roughness = 0.0f;

	gx::mem::addPackageMaterial(&ctx.internalResourceStorage, "materials/default.material", &ctx3d.defaultMaterial, "res/pbr/usedSteel/used-stainless-steel_albedo.png", "res/pbr/usedSteel/used-stainless-steel_normal-ogl.png", "res/pbr/usedSteel/used-stainless-steel_metallic.png", "res/pbr/usedSteel/used-stainless-steel_roughness.png", "res/pbr/usedSteel/used-stainless-steel_ao.png");


	//save
	gx::mem::saveToFile(&ctx.internalResourceStorage, "internalResources.bin");
	gx::mem::clearPackages(&ctx.internalResourceStorage);
}

void context::loadResourcesFromResourceStorage()
{
	ctx.internalResourceStorage = gx::mem::loadFromFile("internalResources.bin");

	//shaders
	ctx2d.primitiveShader = gx::shaderLoad(&ctx.internalResourceStorage, "shaders/primitive.shader");
	ctx2d.circleShader = gx::shaderLoad(&ctx.internalResourceStorage, "shaders/circle.shader");
	ctx2d.textureShader = gx::shaderLoad(&ctx.internalResourceStorage, "shaders/texture.shader");
	ctx3d.gBufferShader = gx::shaderLoad(&ctx.internalResourceStorage, "shaders/gBuffer.shader");
	ctx3d.deferredShader = gx::shaderLoad(&ctx.internalResourceStorage, "shaders/deferred.shader");
	
	//default material
	ctx3d.defaultMaterial = gx::materialLoad(&ctx.internalResourceStorage, "materials/default.material");
	ctx3d.defaultMaterial.name = "default";
	ctx3d.defaultMaterial.color = { 1, 1, 1 };
	ctx3d.defaultMaterial.metallic = 1.0f;
	ctx3d.defaultMaterial.roughness = 0.0f;

	gx::mem::clearPackages(&ctx.internalResourceStorage);
}

void context::initAudio()
{
	ctx.audioDevice = alcOpenDevice(nullptr);
	if (!ctx.audioDevice)
		gx::debug::logError("Could not open audio device");

	ctx.audioContext = alcCreateContext(ctx.audioDevice, nullptr);
	if (!ctx.audioContext)
		gx::debug::logError("Could not create audio context");

	if(!alcMakeContextCurrent(ctx.audioContext))
		gx::debug::logError("Could not set audio context to current");
}

void context::destroy()
{
	glfwDestroyWindow(ctx.window);

	if (!alcCloseDevice(ctx.audioDevice) || !alcMakeContextCurrent(nullptr))
		gx::debug::logError("Could not destroy audio device and or context");
	alcDestroyContext(ctx.audioContext);
}




context2d ctx2d;

void context2d::init()
{
	//Quad Mesh
	float quadVertices[20] = {
		// positions        // texture Coords
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	unsigned int quadIndices[6] = {
	0, 1, 2,  // 1. triangle (up left, down left, up right)
	1, 3, 2   // 2. triangle (down left, down right, up right)
	};

	quad = gx::meshCreate({gx::VertexAttribute::float3, gx::VertexAttribute::float2 });
	gx::meshSetData(&quad, quadVertices, 20, quadIndices, 6);

	//RenderProfile
	renderProfile2D = gx::renderProfileCreate({ gx::renderParameter::blend, gx::renderParameter::anisotropicFiltering, gx::renderParameter::antialiasing, gx::renderParameter::faceCulling });

	//sizes
	glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &batchBufferCapacity);//THIS LIMIT IS FOR EACH BUFFER ONLY !!!
	batchBufferCapacity /= 4;//divide max capacity even and leave enough space ALTHOUGH /\.
	batchCircleBufferCapacity = batchBufferCapacity;
	batchTextureBufferCapacity = batchBufferCapacity;
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &batchTextureSlots);

	//triangles
	batchMesh = gx::meshCreate({ gx::VertexAttribute::float2/*position*/, gx::VertexAttribute::float4/*color*/ });
	//circles
	batchCircleMesh = gx::meshCreate({ gx::VertexAttribute::float2/*position*/, gx::VertexAttribute::float4/*color*/, gx::VertexAttribute::float1/*radius*/, gx::VertexAttribute::float2/*center*/ });
	//textures
	batchTextureMesh = gx::meshCreate({ gx::VertexAttribute::float3/*position*/, gx::VertexAttribute::float4/*color*/, gx::VertexAttribute::float2/*texCoord*/, gx::VertexAttribute::float1/*textureSlot*/, gx::VertexAttribute::float4/*srcRect*/});
}

void context2d::flushAllBatchRenderers()
{
	flushBatchTextureRenderer();
	flushBatchRenderer();
	flushBatchCircleRenderer();
}

void context2d::flushBatchRenderer()
{
	if (ctx2d.batchVertices.size() == 0 || ctx2d.batchIndices.size() == 0) return;//if nothing to render return
	gx::meshSetData(&ctx2d.batchMesh, &ctx2d.batchVertices[0], ctx2d.batchVertices.size(), &ctx2d.batchIndices[0], ctx2d.batchIndices.size());

	gx::shaderBind(&ctx2d.primitiveShader);
	//gx::setShaderUniform(&ctx.rectangleShader, "u_mvp", glm::ortho(-gx::window::getSize().x / 2, gx::window::getSize().x / 2, -gx::window::getSize().y / 2, gx::window::getSize().y / 2));
	gx::shaderSetUniform(&ctx2d.primitiveShader, "u_mvp", gx::common::getCurrentProjectionMatrixOrthographic());
	
	gx::renderProfileUse(&ctx2d.renderProfile2D);
	
	gx::meshDraw(&batchMesh);

	ctx2d.batchVertices.clear();
	ctx2d.batchIndices.clear();
}

void context2d::flushBatchCircleRenderer()
{
	if (ctx2d.batchCircleVertices.size() == 0 || ctx2d.batchCircleIndices.size() == 0) return;//if nothing to render return
	gx::meshSetData(&batchCircleMesh, &ctx2d.batchCircleVertices[0], ctx2d.batchCircleVertices.size(), &ctx2d.batchCircleIndices[0], ctx2d.batchCircleIndices.size());

	gx::shaderBind(&ctx2d.circleShader);
	gx::shaderSetUniform(&ctx2d.circleShader, "u_mvp", gx::common::getCurrentProjectionMatrixOrthographic());

	gx::renderProfileUse(&ctx2d.renderProfile2D);
	gx::meshDraw(&batchCircleMesh);

	ctx2d.batchCircleVertices.clear();
	ctx2d.batchCircleIndices.clear();
}

void context2d::flushBatchTextureRenderer()
{
	if (ctx2d.batchTextureVertices.size() == 0 || ctx2d.batchTextureIndices.size() == 0) return;//if nothing to render return
	gx::meshSetData(&ctx2d.batchTextureMesh, &ctx2d.batchTextureVertices[0], ctx2d.batchTextureVertices.size(), &ctx2d.batchTextureIndices[0], ctx2d.batchTextureIndices.size());
	
	gx::shaderBind(&ctx2d.textureShader);
	gx::shaderSetUniform(&ctx2d.textureShader, "u_mvp", gx::common::getCurrentProjectionMatrixOrthographic());

	int textureArrayLocation = glGetUniformLocation(ctx2d.textureShader.id, "u_textures");

	int textureUnit = 0;
	for (const auto& pair : ctx2d.batchTextureTextures) 
	{
		gx::Texture* texture = pair.second;

		gx::textureBind(texture, textureUnit);

		glUniform1i(textureArrayLocation + textureUnit, textureUnit);
		textureUnit++;
		if (textureUnit >= 32)
			break;
	}
	gx::renderProfileUse(&ctx2d.renderProfile2D);
	gx::meshDraw(&batchTextureMesh);
	gx::textureUnbind();


	ctx2d.batchTextureVertices.clear();
	ctx2d.batchTextureIndices.clear();
	ctx2d.batchTextureTextures.clear();
}

void context2d::beginImGuiRendering()
{
	ImGui_ImplGlfwGL3_NewFrame();
}

void context2d::endImGuiRendering()
{
	ImGui::Render();
	ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void context2d::destroy()
{
	//opengl deletes
	gx::meshDelete(&ctx2d.batchMesh);
	gx::shaderDelete(&ctx2d.primitiveShader);
	gx::meshDelete(&ctx2d.batchCircleMesh);
	gx::shaderDelete(&ctx2d.circleShader);
	gx::meshDelete(&ctx2d.batchTextureMesh);
	gx::shaderDelete(&ctx2d.textureShader);
}




context3d ctx3d;

void context3d::init()
{
	//basic meshes
	cube = gx::meshCreate({ gx::VertexAttribute::float3 });
	float cubeMapVertices[24] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
	};
	unsigned int cubeMapIndices[36] = {
		0, 1, 2,
		2, 3, 0,
		4, 5, 6,
		6, 7, 4,
		0, 1, 5,
		5, 4, 0,
		2, 3, 7,
		7, 6, 2,
		0, 3, 7,
		7, 4, 0,
		1, 2, 6,
		6, 5, 1
	};
	gx::meshSetData(&cube, cubeMapVertices, 24, cubeMapIndices, 36);

	//RenderProfiles
	deferredRenderProfile3D = gx::renderProfileCreate({ gx::renderParameter::depthTest/* at least for gbuffer fill, ... see if needed anywhere else, gx::renderParameter::blend*/, gx::renderParameter::faceCulling});

	//shaders
	gx::shaderBind(&gBufferShader);
	gx::shaderSetUniform(&gBufferShader, "material.colorTexture", 0);
	gx::shaderSetUniform(&gBufferShader, "material.normalTexture", 1);
	gx::shaderSetUniform(&gBufferShader, "material.roughnessTexture", 2);

	gx::shaderBind(&deferredShader);
	gx::shaderSetUniform(&deferredShader, "gPosition", 0);
	gx::shaderSetUniform(&deferredShader, "gNormal", 1);
	gx::shaderSetUniform(&deferredShader, "gAlbedoSpec", 2);

	//framebuffers
	{
		deferredOutput = gx::framebufferCreate();
		gx::framebufferBind(&deferredOutput);
		gx::framebufferAttachTexture(&deferredOutput, gx::FramebufferAttachment::color0, { gx::textureAttributes::repeat, gx::textureAttributes::nearest, gx::textureAttributes::texture2d, gx::textureAttributes::rgb, gx::textureAttributes::uByte, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y , 0 });
		gx::framebufferAttachRenderbuffer(&deferredOutput, gx::FramebufferAttachment::depthStencil, gx::textureAttributes::depth24_stencil8, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y);
		if (!gx::framebufferComplete(&deferredOutput))
			gx::debug::logError("Could not complete framebuffer output for deferred rendering");
		gx::framebufferUnbindSmart();


		deferredFramebuffer = gx::framebufferCreate();
		gx::framebufferBind(&deferredFramebuffer);
		//gPosition
		gx::framebufferAttachTexture(&deferredFramebuffer, gx::FramebufferAttachment::color0, { gx::textureAttributes::repeat, gx::textureAttributes::nearest, gx::textureAttributes::texture2d, gx::textureAttributes::rgb16f, gx::textureAttributes::float32, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y, 0 });
		//gNormal
		gx::framebufferAttachTexture(&deferredFramebuffer, gx::FramebufferAttachment::color1, { gx::textureAttributes::repeat, gx::textureAttributes::nearest, gx::textureAttributes::texture2d, gx::textureAttributes::rgb16f, gx::textureAttributes::float32, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y, 0 });
		//gAlbedoSpec
		gx::framebufferAttachTexture(&deferredFramebuffer, gx::FramebufferAttachment::color2, { gx::textureAttributes::repeat, gx::textureAttributes::nearest, gx::textureAttributes::texture2d, gx::textureAttributes::rgba16f, gx::textureAttributes::uByte, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y, 0});
		//set these to output buffers
		gx::framebufferSetOutputBuffers(&deferredFramebuffer, { gx::FramebufferAttachment::color0, gx::FramebufferAttachment::color1, gx::FramebufferAttachment::color2 });
		gx::framebufferAttachRenderbuffer(&deferredFramebuffer, gx::FramebufferAttachment::depth, gx::textureAttributes::depth24, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y);
		if (!gx::framebufferComplete(&deferredFramebuffer))
			gx::debug::logError("Could not complete framebuffer for deferred rendering");
		gx::framebufferUnbindSmart();
	}

	//defaults
	defaultTransform = gx::matrixCreate();
}

void context3d::flushDeferredRenderer()
{
	if (ctx3d.drawCallQueueDeferred.size() == 0)
		return;

	//DANGEROUS TODO every callback not frame
	gx::framebufferResize(&ctx3d.deferredOutput, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y);
	gx::framebufferResize(&ctx3d.deferredFramebuffer, (unsigned int)gx::window::getSize().x, (unsigned int)gx::window::getSize().y);

	//first pass
	{
		gx::renderProfileUse(&ctx3d.deferredRenderProfile3D);
		gx::framebufferBind(&ctx3d.deferredFramebuffer);

		//TODO use gx::renderer2d::clear(); but mpve function out of 2d ! it's universal
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		gx::shaderBind(&gBufferShader);
		gx::shaderSetUniform(&gBufferShader, "projection", gx::common::getCurrentProjectionMatrixPerspective());
		gx::shaderSetUniform(&gBufferShader, "view", *gx::camera::getViewMatrixPtr());

		for (size_t i = 0; i < ctx3d.drawCallQueueDeferred.size(); i++)
		{
			gx::shaderSetUniform(&gBufferShader, "model", *ctx3d.drawCallQueueDeferred[i].transform);

			//bind textures if given, TODO if not EITHER use other attribs like color (set uniform to inform shader) OR use defaultMaterial texture where missing

			if (ctx3d.drawCallQueueDeferred[i].material->colorTexture != nullptr)
				gx::textureBind(ctx3d.drawCallQueueDeferred[i].material->colorTexture, 0);
			else
				gx::textureBind(ctx3d.defaultMaterial.colorTexture, 0);

			if (ctx3d.drawCallQueueDeferred[i].material->normalTexture != nullptr)
				gx::textureBind(ctx3d.drawCallQueueDeferred[i].material->normalTexture, 1);
			else
				gx::textureBind(ctx3d.defaultMaterial.normalTexture, 1);

			if (ctx3d.drawCallQueueDeferred[i].material->roughnessTexture != nullptr)
				gx::textureBind(ctx3d.drawCallQueueDeferred[i].material->roughnessTexture, 2);
			else
				gx::textureBind(ctx3d.defaultMaterial.roughnessTexture, 2);

			gx::meshDraw(ctx3d.drawCallQueueDeferred[i].mesh);
		}

		gx::framebufferUnbindSmart();
	}
	

	//second pass
	{
		//TODO using ctx2d.quad. 3d depends on 2d. Should this be ?

		gx::framebufferBind(&deferredOutput);
		//TODO use gx::renderer2d::clear(); but move function out of 2d ! it's universal
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		gx::shaderBind(&deferredShader);

		//set light uniforms
		gx::shaderSetUniform(&deferredShader, "viewPos", *gx::camera::getPositionPtr());
		{
			gx::shaderSetUniform(&deferredShader, "dirLight.direction", ctx3d.deferredEnvironment.directionalLight.direction);
			gx::shaderSetUniform(&deferredShader, "dirLight.ambient", ctx3d.deferredEnvironment.directionalLight.ambient);
			gx::shaderSetUniform(&deferredShader, "dirLight.diffuse", ctx3d.deferredEnvironment.directionalLight.diffuse);
			gx::shaderSetUniform(&deferredShader, "dirLight.specular", ctx3d.deferredEnvironment.directionalLight.specular);
			gx::shaderSetUniform(&deferredShader, "dirLight.state", ctx3d.deferredEnvironment.directionalLight.state);

			for (unsigned int i = 0; i < ctx3d.deferredEnvironment.pointLights.size(); i++)
			{
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].position").c_str(), ctx3d.deferredEnvironment.pointLights[i].position);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].constant").c_str(), ctx3d.deferredEnvironment.pointLights[i].constant);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].linear").c_str(), ctx3d.deferredEnvironment.pointLights[i].linear);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].quadratic").c_str(), ctx3d.deferredEnvironment.pointLights[i].quadratic);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].ambient").c_str(), ctx3d.deferredEnvironment.pointLights[i].ambient);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].diffuse").c_str(), ctx3d.deferredEnvironment.pointLights[i].diffuse);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].specular").c_str(), ctx3d.deferredEnvironment.pointLights[i].specular);
				gx::shaderSetUniform(&deferredShader, std::string("pointLights[" + std::to_string(i) + "].state").c_str(), ctx3d.deferredEnvironment.pointLights[i].state);
			}

			//TODO spot lights as well
		}

		for (size_t i = 0; i < ctx3d.deferredFramebuffer.textureAttachements.size(); i++)
			gx::textureBind(&ctx3d.deferredFramebuffer.textureAttachements[i], i);


		//set faceCulling false for this drawcall
		ctx3d.deferredRenderProfile3D.parameters[gx::renderParameter::faceCulling] = false;
		gx::renderProfileUse(&ctx3d.deferredRenderProfile3D);
		ctx3d.deferredRenderProfile3D.parameters[gx::renderParameter::faceCulling] = true;

		gx::meshDraw(&ctx2d.quad);

		gx::framebufferUnbindSmart();
	}


	ctx3d.drawCallQueueDeferred.clear();
}

void context3d::destroy()
{
	
}