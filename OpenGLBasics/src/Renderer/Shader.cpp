#include "Shader.h"
#include "Renderer.h"
#include "Core/Log.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

namespace glb {

    Shader::Shader(const std::string& filepath)
        : m_Filepath(filepath), m_RendererID(0) {

        ShaderProgramSource source = ParseShader(filepath);
        m_RendererID = CreateShader(source.VertexSource, source.FragementSource);
    }

    Shader::~Shader() {
        GLCALL(glDeleteProgram(m_RendererID));
    }

    unsigned int Shader::CompileShader(unsigned int type, const std::string& source) {

        GLCALL(unsigned int id = glCreateShader(type));
        const char* src = source.c_str();
        GLCALL(glShaderSource(id, 1, &src, nullptr));
        GLCALL(glCompileShader(id));
        int result;
        GLCALL(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

        if (result == GL_FALSE) {
            int length;
            GLCALL(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
            char* messege = (char*)alloca(length * sizeof(char));
            GLCALL(glGetShaderInfoLog(id, length, &length, messege));
            std::cout << messege << std::endl;
            GLCALL(glDeleteShader(id));

            return 0;
        }

        return id;
    }

    unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {

        GLCALL(unsigned int program = glCreateProgram());

        unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
        unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

        GLCALL(glAttachShader(program, vs));
        GLCALL(glAttachShader(program, fs));
        GLCALL(glLinkProgram(program));
        GLCALL(glValidateProgram(program));

        GLCALL(glDeleteShader(vs));
        GLCALL(glDeleteShader(fs));

        return program;
    }

    ShaderProgramSource Shader::ParseShader(const std::string& filepath) {

        std::fstream stream(filepath);

        enum class ShaderType {
            NONE = -1,
            VERTEX = 0,
            FRAGMENT = 1
        };

        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;

        while (getline(stream, line)) {

            if (line.find("#shader") != std::string::npos) {

                if (line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;

                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;

            }
            else {
                ss[(int)type] << line << '\n';
            }
        }

        return { ss[0].str(), ss[1].str() };
    }

    unsigned int Shader::GetUniformLocation(const std::string& name) {

        if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
            return m_UniformLocationCache[name];

        GLCALL(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));


        if (location == -1)
            GLB_WARN("Uniform {0} does not exist.", name);

        m_UniformLocationCache[name] = location;

        return location;
    }

    void Shader::Bind() const {

        GLCALL(glUseProgram(m_RendererID));
    }

    void Shader::Unbind() const {

        GLCALL(glUseProgram(0));
    }


    void Shader::SetUniform1f(const std::string& name, float f) {
        GLCALL(glUniform1f(GetUniformLocation(name), f));
    }

    void Shader::SetUniform3f(const std::string& name, float v0, float v1, float v2) {
        GLCALL(glUniform3f(GetUniformLocation(name), v0, v1, v2));
    }

    void Shader::SetUniform3f(const std::string& name, glm::vec3& uniform) {
        GLCALL(glUniform3fv(GetUniformLocation(name), 1, &uniform[0]));
    }

    void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) {
        GLCALL(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
    }

    void Shader::SetUniform4f(const std::string& name, glm::vec4& uniform) {
        GLCALL(glUniform4fv(GetUniformLocation(name), 1, &uniform[0]));
    }

    void Shader::SetUniformMat4(const std::string& name, const glm::mat4& matrix) {
        GLCALL(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix)));
    }
}