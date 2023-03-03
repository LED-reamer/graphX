#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace gx
{
	struct PrimitiveVertex
	{
		glm::vec2 pos;
		glm::vec4 col;
	};

	struct CircleVertex
	{
		glm::vec2 pos;
		glm::vec4 col;
		float radius;
		glm::vec2 center;
	};

	struct VertexBuffer
	{
		unsigned int VA;
		unsigned int VB;
		unsigned int EB;
	};

	struct Shader
	{
		unsigned int id;
		std::string name;
		std::string path;
	};

	Shader loadShader(std::string path, std::string name = "untitled shader");
	Shader loadShaderFromString(std::string sourceVertex, std::string sourceFragment, std::string name = "untitled shader");
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
}