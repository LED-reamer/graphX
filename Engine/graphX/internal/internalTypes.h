#pragma once
#include <vector>
#include <string>

#include "graphX/vendor/glm/glm.hpp"

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

	struct TextureVertex3d
	{
		glm::vec3 pos;
		glm::vec4 col;
		glm::vec2 texCoord;
		float textureSlot;
		glm::vec4 srcRec;

	};

	namespace mem
	{
		struct memoryMesh;
		struct memoryModel;
		struct memoryMaterial;
		struct memoryShader;
		struct memoryTexture;
		struct memorySound;


		struct memoryMesh
		{
			size_t vertexLayoutSize;
			std::vector<float> vertices;
			std::vector<unsigned int> indices;
		};

		struct memoryModel
		{
			std::vector<gx::mem::memoryMesh> submeshes;
			std::vector<int> materialIds;
			std::vector<gx::mem::memoryMaterial> materials;
			std::vector<gx::mem::memoryTexture> textures;

			std::string desc;
		};

		struct memoryMaterial
		{
			std::string name;

			float colorR;
			float colorG;
			float colorB;
			float metallic;
			float roughness;
		};

		struct memoryShader
		{
			std::vector<std::string> sources;
		};

		struct memoryTexture
		{
			unsigned int target;
			unsigned int formatInternal;
			unsigned int dataType;
			unsigned int wrapping;
			unsigned int filtering;
			unsigned int format;//of the texture file
			int width;
			int height;
			int depth;
			int channels;
			int levels;

			std::vector<uint8_t> data;
		};

		struct memorySound
		{
			unsigned int channels;
			unsigned int sampleRate;
			unsigned int bitsPerSample;
			int format;
			std::vector<float> data;
			//std::vector<std::pair<unsigned int, float[3]>> attributes;
		};
	}

	namespace renderer3d
	{
		struct DrawCall
		{
			gx::Mesh* mesh;
			gx::Matrix* transform;
			gx::Material* material;
		};
	}
}