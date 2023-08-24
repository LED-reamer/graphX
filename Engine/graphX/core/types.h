#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "graphX/vendor/glm/glm.hpp"
#include "graphX/vendor/openglIncludes.h"

#define CHECK_OPENGL_ERROR() \
    do { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            std::cout << "OpenGL Error (" << __FILE__ << ":" << __LINE__ << "): " << error << std::endl; \
        } \
    } while (false); \


namespace gx
{
	//declarations
	struct MemoryStorage;
	struct Mesh;
	struct Material;
	struct Model;
	struct Shader;
	struct Renderbuffer;
	struct Framebuffer;
	struct RenderTarget;
	struct Texture;
	struct CubeMap;
	struct RenderProfile;
	struct Sound;


	//
	//  VECTOR
	//

	typedef glm::vec2 Vec2;
	typedef glm::vec3 Vec3;
	typedef glm::vec4 Vec4;

	//
	//  MATRIX
	//

	typedef glm::mat4 Matrix;
	Matrix matrixCreate(float x = 1.0f);
	Matrix matrixTranslate(Matrix* matrix, Vec3 translation);
	Matrix matrixRotate(Matrix* matrix, Vec3 rotation);
	Matrix matrixScale(Matrix* matrix, Vec3 scale);
	Matrix matrixLookAt(Vec3 position, Vec3 target, Vec3 up = Vec3{0, 1, 0});
	Matrix matrixProjectOrthographic(float leftX, float rightX, float bottomY, float topY, float nearZ, float farZ);
	Matrix matrixProjectPerspective(float fov, float nearZ, float farZ, unsigned int width, unsigned int height);

	//
	//  STORAGE
	//

	struct MemoryStorage
	{
		std::vector<std::pair<std::string, std::vector<uint8_t>>> packages;
	};

	//
	//  MESH
	//

	enum VertexAttribute
	{
		float1,
		float2,
		float3,
		float4,
		int1,
		uint1,

		//normalized
		float1Norm,
		float2Norm,
		float3Norm,
		float4Norm,
		int1Norm,
		uint1Norm,
	};

	struct Mesh
	{
		Mesh()
		{
			VA = 0;
			VB = 0;
			EB = 0;
			vertexLayoutSize = 0;
			indicesCount = 0;
		}

		unsigned int VA;
		unsigned int VB;
		unsigned int EB;

		size_t vertexLayoutSize;
		unsigned int indicesCount;
	};

	Mesh meshCreate(std::vector<VertexAttribute> vertexAttributes);
	//TODO add Mesh meshLoadFromFile(Mesh* mesh, std::string path, unsigned int meshIndex = 0);
	void meshSetData(Mesh* mesh, void* vertices, size_t length0, unsigned int* indices, size_t length1);
	void meshDraw(Mesh* mesh);
	void meshDrawInstanced(Mesh* mesh, unsigned int count);
	void meshDelete(Mesh* mesh);

	//
	//  MATERIAL
	//

	struct Material
	{
		Material()
		{
			name = "untitled material";

			color = { 1, 1, 1 };
			metallic = 1.0f;
			roughness = 0.0f;

			colorTexture = nullptr;
			normalTexture = nullptr;
			metallicTexture = nullptr;
			roughnessTexture = nullptr;
			ambientOcclusionTexture = nullptr;
		}

		std::string name;

		gx::Vec3 color;
		float metallic;
		float roughness;

		gx::Texture* colorTexture;
		gx::Texture* normalTexture;
		gx::Texture* metallicTexture;
		gx::Texture* roughnessTexture;
		gx::Texture* ambientOcclusionTexture;
	};

	Material materialLoad(gx::MemoryStorage* storage, std::string virtualPath);

	//
	//  MODEL
	//

	struct Model
	{
		std::vector<gx::Mesh> submeshes;
		std::vector<unsigned int> materialIds;
		std::vector<gx::Material> materials;
		std::vector<gx::Texture> textures;

		std::string desc;
	};

	gx::Model modelLoadFromFile(std::string path);
	//TODO: add void modelLoadFromFileAsync(std::string path, gx::Model* model);
	void modelDraw(Model* model);
	void modelDrawInstanced(Model* model, unsigned int count);
	void modelDelete(Model* model);

	//
	//  SHADER
	//

	struct Shader
	{
		unsigned int id;
		std::string path;
	};

	Shader shaderLoadFromFile(std::string path);
	Shader shaderLoad(gx::MemoryStorage* storage, std::string virtualPath);
	Shader shaderLoadFromString(std::string vertexSource, std::string fragmentSource, std::string geometrySource = "", std::string tessControlSource = "", std::string tessEvalSource = "", std::string computeSource = "");
	void shaderBind(Shader* shader);
	void shaderDelete(Shader* shader);

	//TODO: change to pointers or references !!! Do not copy matrices all the time
	void shaderSetUniform(Shader* shader, const char* uniformName, float float0);
	void shaderSetUniform(Shader* shader, const char* uniformName, int int0);
	void shaderSetUniform(Shader* shader, const char* uniformName, unsigned int uInt0);
	void shaderSetUniform(Shader* shader, const char* uniformName, std::vector<float> vector0);
	void shaderSetUniform(Shader* shader, const char* uniformName, std::vector<int> vector0);
	void shaderSetUniform(Shader* shader, const char* uniformName, Matrix mat4);
	void shaderSetUniform(Shader* shader, const char* uniformName, Vec2 vec2);
	void shaderSetUniform(Shader* shader, const char* uniformName, Vec3 vec3);
	void shaderSetUniform(Shader* shader, const char* uniformName, Vec4 vec4);

	//
	//  TEXTURE
	//

	namespace textureAttributes
	{
		enum wrapping
		{
			repeat = GL_REPEAT,
			mirroredRepeat = GL_MIRRORED_REPEAT,
			clampToEdge = GL_CLAMP_TO_EDGE,
			clampToBorder = GL_CLAMP_TO_BORDER
		};

		enum filtering
		{
			nearest = GL_NEAREST,//pixelated
			linear = GL_LINEAR//interpolated
		};

		enum target
		{
			texture1d = GL_TEXTURE_1D,
			texture2d = GL_TEXTURE_2D,
			texture3d = GL_TEXTURE_3D,
			textureRectangle = GL_TEXTURE_RECTANGLE,
			textureBuffer = GL_TEXTURE_BUFFER,
			textureCubeMap = GL_TEXTURE_CUBE_MAP,
			texture1dArray = GL_TEXTURE_1D_ARRAY,
			texture2dArray = GL_TEXTURE_2D_ARRAY,
			textureCubeMapArray = GL_TEXTURE_CUBE_MAP_ARRAY,
			texture2dMultisample = GL_TEXTURE_2D_MULTISAMPLE,
			texture2dMultisampleArray = GL_TEXTURE_2D_MULTISAMPLE_ARRAY
		};

		enum format
		{
			automatically = 0,
			rgb = GL_RGB,
			rgba = GL_RGBA,
			red = GL_RED,
			green = GL_GREEN,
			blue = GL_BLUE,
			bgr = GL_BGR,

			depth16 = GL_DEPTH_COMPONENT16,
			depth24 = GL_DEPTH_COMPONENT24,
			depth32 = GL_DEPTH_COMPONENT32,
			depth24_stencil8 = GL_DEPTH24_STENCIL8,
			depth32F_stencil8 = GL_DEPTH32F_STENCIL8,

			r8 = GL_R8,
			rg8 = GL_RG8,
			rgb8 = GL_RGB8,
			rgba8 = GL_RGBA8,
			r16f = GL_R16F,
			rg16f = GL_RG16F,
			rgb16f= GL_RGB16F,
			rgba16f = GL_RGBA16F,
			r32f = GL_R32F,
			rg32f = GL_RG32F,
			rgb32f = GL_RGB32F,
			rgba32f = GL_RGBA32F
		};

		enum dataType
		{
			float32 = GL_FLOAT,
			uByte = GL_UNSIGNED_BYTE,
			byte = GL_BYTE,
			uInt = GL_UNSIGNED_INT,
			int32 = GL_INT,
		};
	}

	struct TextureDescription
	{
		textureAttributes::wrapping wrapping;
		textureAttributes::filtering filtering;
		textureAttributes::target target;
		textureAttributes::format format;
		textureAttributes::dataType dataType;

		unsigned int width;
		unsigned int height;
		unsigned int depth;
	};

	struct Texture
	{
		Texture()
		{
			id = 0;
			target = textureAttributes::target::texture1d;
			format = textureAttributes::format::red;
			dataType = textureAttributes::dataType::byte;
			width = 0;
			height = 0;
			depth = 0;
			levels = 0;
		}

		unsigned int id;
		textureAttributes::target target;//GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP
		textureAttributes::format format;//GL_RGBA, GL_RGB, GL_DEPTH_COMPONENT
		textureAttributes::dataType dataType;//GL_FLOAT
		int width;
		int height;
		int depth;//3rd dimension textures
		int levels;//level of mip mapping
	};
	

	Texture textureCreate(TextureDescription* description);
	Texture textureCreate(textureAttributes::target target = textureAttributes::target::texture2d, textureAttributes::wrapping wrapping = textureAttributes::wrapping::repeat, textureAttributes::filtering filtering = textureAttributes::filtering::nearest, textureAttributes::format formatInternal = textureAttributes::format::rgba8, textureAttributes::dataType dataType = textureAttributes::dataType::uByte, int width = 1, int height = 1, int depth = 1);
	Texture textureLoadFromFile(std::string path, gx::textureAttributes::format imageFormat = gx::textureAttributes::format::automatically);
	Texture textureLoad(gx::MemoryStorage* storage, std::string virtualPath);
	void textureLoadImage(Texture* texture, std::string path, gx::textureAttributes::format imageFormat = gx::textureAttributes::format::automatically);
	void textureLoadImage(Texture* texture, unsigned char* data, int width, int height, int channels, gx::textureAttributes::format imageFormat = gx::textureAttributes::format::automatically);
	void textureSetAttribute(Texture* texture, textureAttributes::wrapping newWrapping);
	void textureSetAttribute(Texture* texture, textureAttributes::filtering newFiltering);
	void textureSetBorderColor(Texture* texture, Vec4 color);
	void textureResize(Texture* texture, int width, int height);
	void textureBind(Texture* texture, unsigned int slot = 0);
	void textureUnbind();
	// TODO add void textureDraw(); //completeness for non batching?
	void textureDelete(Texture* texture);

	//
	//  CUBEMAP
	//

	struct CubeMap
	{
		std::vector<gx::Texture> textures;
		gx::Mesh* cube;
	};

	//textures: x+ x- y+ y- z+ z-
	CubeMap cubeMapCreate(gx::Texture textureXP, gx::Texture textureXN, gx::Texture textureYP, gx::Texture textureYN, gx::Texture textureZP, gx::Texture textureZN);

	//
	//  RENDERBUFFER
	//

	struct Renderbuffer
	{
		Renderbuffer()
		{
			id = 0;
			format = gx::textureAttributes::rgba8;
		}

		unsigned int id;
		gx::textureAttributes::format format;
	};

	Renderbuffer renderbufferCreate(unsigned int width = 0, unsigned int height = 0, gx::textureAttributes::format format = gx::textureAttributes::rgba8);
	void renderbufferSetStorage(Renderbuffer* renderbuffer, unsigned int width, unsigned int height, gx::textureAttributes::format format);
	void renderbufferDelete(Renderbuffer* renderbuffer);

	//
	//  FRAMEBUFFER
	//

	namespace FramebufferAttachment
	{
		enum type
		{
			color0 = GL_COLOR_ATTACHMENT0,
			color1 = GL_COLOR_ATTACHMENT1,
			color2 = GL_COLOR_ATTACHMENT2,
			color3 = GL_COLOR_ATTACHMENT3,
			color4 = GL_COLOR_ATTACHMENT4,
			color5 = GL_COLOR_ATTACHMENT5,
			color6 = GL_COLOR_ATTACHMENT6,
			color7 = GL_COLOR_ATTACHMENT7,
			color8 = GL_COLOR_ATTACHMENT8,
			color9 = GL_COLOR_ATTACHMENT9,
			color10 = GL_COLOR_ATTACHMENT10,
			color11 = GL_COLOR_ATTACHMENT11,
			color12 = GL_COLOR_ATTACHMENT12,
			color13 = GL_COLOR_ATTACHMENT13,
			color14 = GL_COLOR_ATTACHMENT14,
			color15 = GL_COLOR_ATTACHMENT15,

			depth = GL_DEPTH_ATTACHMENT,
			stencil = GL_STENCIL_ATTACHMENT,
			depthStencil = GL_DEPTH_STENCIL_ATTACHMENT,
		};


	}

	struct Framebuffer
	{
		Framebuffer()
		{
			id = 0;
		}
		unsigned int id;
		std::vector<Texture> textureAttachements;
		std::vector<Renderbuffer> renderbufferAttachements;
		std::vector<FramebufferAttachment::type> renderbufferAttachementTypes; //same order as renderbufferAttachments. Needed since renderbuffers need to update the attachment when resizing
	};

	Framebuffer framebufferCreate();
	void framebufferBind(Framebuffer* framebuffer);
	void framebufferResize(Framebuffer* framebuffer, int width, int height);
	void framebufferUnbindSmart();
	void framebufferUnbindAll();
	void framebufferAttachTexture(Framebuffer* framebuffer, FramebufferAttachment::type attachment, TextureDescription description);
	void framebufferAttachRenderbuffer(Framebuffer* framebuffer, FramebufferAttachment::type attachment, gx::textureAttributes::format format, unsigned int width, unsigned int height);
	void framebufferSetOutputBuffers(Framebuffer* framebuffer, std::vector<FramebufferAttachment::type> attachments);
	bool framebufferComplete(Framebuffer* framebuffer);
	void framebufferDelete(Framebuffer* framebuffer); // important delete attachments !!!!!!

	//
	//  RENDERTARGET
	//

	struct RenderTarget
	{
		Framebuffer framebuffer;
		Texture* texturePtr;
	};

	RenderTarget renderTargetCreate(int width, int height);
	void renderTargetResize(RenderTarget* target, int width, int height);
	void renderTargetBind(RenderTarget* target);
	void renderTargetUnbind();
	void renderTargetUnbindAll();
	void renderTargetDelete(RenderTarget* target);

	//
	//  RENDERPROFILE
	//

	namespace renderParameter
	{
		enum param
		{
			wireframe,
			depthTest,
			blend,
			anisotropicFiltering,
			antialiasing,

			faceCulling,
			hideFrontFaces,//standard is Back
			frontFaceCW, //standard is CounterClockWise
		};
	}

	struct RenderProfile
	{
		std::unordered_map<gx::renderParameter::param, bool> parameters;
	};

	RenderProfile renderProfileCreate(std::vector<renderParameter::param> renderParameters = {});
	void renderProfileUse(RenderProfile* newRenderProfile);

	//
	//  SOUND
	//

	enum SourceAttribute
	{
		pitch,
		gain,
		position,
		velocity,
		direction,
		maxDistance,
		distanceLoss,
		coneOuterGain,
		coneInnerAngle,
		coneOuterAngle,
		doppler,
		dopplerVelocity,
		secondsOffset,
		looping,
	};

	struct Sound
	{
		Sound()
		{
			buffer = 0;
			source = 0;
		}
		unsigned int buffer;
		unsigned int source;
	};

	gx::Sound soundLoad(float* sampleData, size_t dataCount, unsigned int sampleRate, int format);
	Sound soundLoad(gx::MemoryStorage* storage, std::string virtualPath);
	gx::Sound soundLoadWAV(std::string path);
	void soundDelete(gx::Sound* sound);
	void soundPlay(gx::Sound* sound);
	void soundStop(gx::Sound* sound);
	void soundSetAttributes(gx::Sound* sound, std::vector<std::pair<SourceAttribute, Vec3>> attributes);
	bool soundIsPlaying(gx::Sound* sound);
}