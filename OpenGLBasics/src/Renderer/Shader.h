#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace glb {

	struct ShaderProgramSource {

		std::string VertexSource;
		std::string FragementSource;
	};

	class Shader {

	private:
		std::string m_Filepath;
		unsigned int m_RendererID;
		std::unordered_map<std::string, int> m_UniformLocationCache;

	public:
		Shader() = default;
		Shader(const std::string& filepath);
		~Shader();

		void Bind() const;
		void Unbind() const;

	private:
		ShaderProgramSource ParseShader(const std::string& filepath);
		unsigned int GetUniformLocation(const std::string& name);
		unsigned int CompileShader(unsigned int type, const std::string& source);
		unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

	public:
		void SetUniform1f(const std::string& name, float f);
		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniform3f(const std::string& name, glm::vec3& uniform);
		void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
		void SetUniform4f(const std::string& name, glm::vec4& uniform);
		void SetUniformMat4(const std::string& name, const glm::mat4& matrix);
	};
}