#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <list>

#include "graphX/vendor/openglIncludes.h"
#include "graphX/vendor/glm/glm.hpp"
#include "graphX/vendor/AL/al.h"
#include "graphX/vendor/AL/alc.h"

#include "graphX/core/types.h"
#include "graphX/core/3d/renderer3d.h"

#include "graphX/internal/internalTypes.h"


struct context
{
	void init();

	//engine settings
	bool quitting;
	bool debugMode;

	//engine resources
	gx::MemoryStorage internalResourceStorage;
	void loadResources();// calls the following 2 functions depending on state
	void saveResourcesFromFiles();
	void loadResourcesFromResourceStorage();

	//glfw
	GLFWwindow* window;


	//audio
	void initAudio();
	ALCdevice* audioDevice;
	ALCcontext* audioContext;

	//camera
	gx::Vec3 cameraPosition;
	gx::Vec3 cameraDirection;
	gx::Vec3 cameraUp;
	gx::Matrix cameraViewMatrix;

	//general rendering
	std::unique_ptr<gx::RenderProfile> currentRenderProfile;
	std::vector<gx::RenderTarget*> currentRenderTargets;
	std::unordered_map<std::string, int> cachedShaderUniformLocations;

	void destroy();
};
extern context ctx;

struct context2d
{
	void init();

	//quad mesh
	gx::Mesh quad;

	gx::RenderProfile renderProfile2D;

	gx::Shader primitiveShader;
	gx::Shader circleShader;
	gx::Shader textureShader;


	//triangles
	int batchBufferCapacity;
	gx::Mesh batchMesh;
	std::vector<gx::PrimitiveVertex2d> batchVertices;
	std::vector<unsigned int> batchIndices;

	//circles
	int batchCircleBufferCapacity;
	gx::Mesh batchCircleMesh;
	std::vector<gx::CircleVertex2d> batchCircleVertices;
	std::vector<unsigned int> batchCircleIndices;

	//textures
	int batchTextureBufferCapacity;
	int batchTextureSlots;
	gx::Mesh batchTextureMesh;
	std::vector<gx::TextureVertex3d> batchTextureVertices;
	std::vector<unsigned int> batchTextureIndices;
	std::map<float, gx::Texture*> batchTextureTextures;

	void flushAllBatchRenderers();
	void flushBatchRenderer();
	void flushBatchCircleRenderer();
	void flushBatchTextureRenderer();

	//imgui
	void beginImGuiRendering();
	void endImGuiRendering();

	void destroy();
};
extern context2d ctx2d;



struct context3d // TODO resize rendertargets and buffers on window callback not every frame
{
	void init();

	//basic meshes
	gx::Mesh cube;

	//deferred
	gx::RenderProfile deferredRenderProfile3D;
	gx::renderer3d::DeferredEnvironment deferredEnvironment;
	gx::Shader gBufferShader;
	gx::Shader deferredShader;
	gx::Framebuffer deferredFramebuffer;
	gx::Framebuffer deferredOutput;
	std::vector<gx::renderer3d::DrawCall> drawCallQueueDeferred;


	gx::Matrix defaultTransform;
	gx::Material defaultMaterial;
	//textures for defaultMaterial
	std::vector<gx::Texture> defaultMaterialTextures;


	void flushDeferredRenderer();

	void destroy();
};
extern context3d ctx3d;