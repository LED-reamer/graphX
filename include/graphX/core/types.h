#pragma once

#include <string>
#include <vector>

#include "graphX/vendor/glm/glm.hpp"
#include "graphX/vendor/openglInclude.h"

#define CHECK_OPENGL_ERROR() \
    do { \
        GLenum error = glGetError(); \
        if (error != GL_NO_ERROR) { \
            std::cout << "OpenGL Error (" << __FILE__ << ":" << __LINE__ << "): " << error << std::endl; \
        } \
    } while (false); \


namespace gx
{
	struct PrimitiveVertex2d
	{
		glm::vec2 pos;
		glm::vec4 col;
	};

	struct CircleVertex2d
	{
		glm::vec2 pos;
		glm::vec4 col;
		float radius;
		glm::vec2 center;
	};

	struct TextureVertex2d
	{
		glm::vec2 pos;
		glm::vec4 col;
		glm::vec2 texCoord;
		float textureSlot;
		glm::vec4 srcRec;

	};

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

	Mesh createMesh(std::vector<VertexAttribute> vertexAttributes);
	void setMeshData(Mesh* mesh, void* vertices, size_t size0, unsigned int* indices, size_t size1);
	void drawMesh(Mesh* mesh);
	void deleteMesh(Mesh* mesh);

	//
	//  SHADER
	//

	struct Shader
	{
		unsigned int id;
		std::string path;
	};

	Shader loadShader(std::string path);
	Shader loadShaderFromString(std::string vertexSource, std::string fragmentSource, std::string geometrySource = "", std::string tessControlSource = "", std::string tessEvalSource = "", std::string computeSource = "");
	void useShader(Shader* shader);
	void deleteShader(Shader* shader);
	void setShaderUniform(Shader* shader, const char* UniformName, float float0);
	void setShaderUniform(Shader* shader, const char* UniformName, int int0);
	void setShaderUniform(Shader* shader, const char* UniformName, unsigned int uInt0);
	void setShaderUniform(Shader* shader, const char* UniformName, std::vector<float> vector0);
	void setShaderUniform(Shader* shader, const char* UniformName, std::vector<int> vector0);
	void setShaderUniform(Shader* shader, const char* UniformName, glm::mat4 mat4);
	void setShaderUniform(Shader* shader, const char* UniformName, glm::vec2 vec2);
	void setShaderUniform(Shader* shader, const char* UniformName, glm::vec3 vec3);
	void setShaderUniform(Shader* shader, const char* UniformName, glm::vec4 vec4);

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
		};

		enum formatInternal
		{
			rgb8 = GL_RGB8,
			rgba8 = GL_RGBA8,
			byte = GL_RED
		};

		enum dataType
		{
			float32 = GL_FLOAT,
			uByte = GL_UNSIGNED_BYTE,
			Byte = GL_BYTE,
			uInt = GL_UNSIGNED_INT,
			int32 = GL_INT,
		};
	}

	struct Texture
	{
		Texture()
		{
			id = 0;
			target = 0;
			format = 0;
			dataType = 0;
			width = 0;
			height = 0;
			depth = 0;
			levels = 0;
		}

		unsigned int id;
		unsigned int target;//GL_TEXTURE_2D, GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP
		unsigned int format;//GL_RGBA, GL_RGB, GL_DEPTH_COMPONENT
		unsigned int dataType;
		int width;
		int height;
		int depth;//like width and height but when using 3d textures
		int levels;//level of mip mapping
	};
	

	Texture createTexture(textureAttributes::target target = textureAttributes::target::texture2d, textureAttributes::wrapping wrapping = textureAttributes::wrapping::repeat, textureAttributes::filtering filtering = textureAttributes::filtering::nearest, textureAttributes::formatInternal formatInternal = textureAttributes::formatInternal::rgba8, textureAttributes::dataType dataType = textureAttributes::dataType::uByte, int width = 1, int height = 1, int depth = 1);
	Texture loadImage(std::string path, gx::textureAttributes::format imageFormat = gx::textureAttributes::format::automatically);
	void loadImage(Texture* texture, std::string path, gx::textureAttributes::format imageFormat = gx::textureAttributes::format::automatically);
	void resizeTexture(Texture* texture, int width, int height);
	void bindTexture(Texture* texture, unsigned int slot = 0);
	void unBindTexture();
	void deleteTexture(Texture* texture);

	//
	//  RENDERTARGET
	//

	struct RenderTarget
	{
		RenderTarget()
		{
			id = 0;
			rbo = 0;
			bound = false;
		}

		Texture texture;
		unsigned int id;//fbo
		unsigned int rbo;
		bool bound;
	};

	RenderTarget createRenderTarget(int width, int height);
	void resizeRenderTarget(RenderTarget* target, int width, int height);
	void bindRenderTarget(RenderTarget* target);
	void unBindRenderTarget();
	void deleteRenderTarget(RenderTarget* target);
}