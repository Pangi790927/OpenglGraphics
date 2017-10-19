#ifndef SHADERPROGRAM_H_INCLUDED
#define SHADERPROGRAM_H_INCLUDED

#include "Shader.h"
#include <vector>
#include <memory>

class ShaderProgram {
public:
	bool isProgramValid = false;
	GLuint program; 
	std::vector<Shader> shaders;

	ShaderProgram () {}
	
	ShaderProgram (std::initializer_list<std::pair<GLenum, std::string>> list) {
		for (auto&& shaderPair : list) {
			try {
				addShader(shaderPair.first, shaderPair.second);
			}
			catch (const std::exception& except) {
				deleteProgram();
				deleteShaders();

				throw std::runtime_error(std::string() + "program shader creation failed because" \
											   " shader failed  EXCEPTION: " + except.what());  
			}
		}

		createProgram(); 
		attachShaders(); 
	}

 	void clear() {
		deleteProgram();
		deleteShaders();
	}

	void useProgram() {
		glUseProgram(program);
	}

	void createProgram() {
		program = glCreateProgram();
		isProgramValid = true; 
	}

	void attachShaders() {
		for (auto&& shader : shaders) {
			glAttachShader(program, shader.getShader());
		}
	}

	void detachShaders() {
		for (auto&& shader : shaders) {
			glDetachShader(program, shader.getShader());
		}
	}

	void linkProgram() {
		glLinkProgram(program);

		if (hasErrors())
			throw std::runtime_error("program shader linking failed"); 

		detachShaders();
	}

	void addShader (GLenum type, std::string fileName) {
		shaders.push_back(Shader(type, fileName));
	}

	bool hasErrors() {
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);

		if (isLinked == GL_FALSE) {
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::unique_ptr<char[]> infoLog(new char[maxLength]);  
			glGetProgramInfoLog(program, maxLength, &maxLength, infoLog.get());
			
			ERROR_PRINT(std::string(infoLog.get()), 0, 0);
			
			deleteProgram();
			deleteShaders();

			return true; 
		}

		return false; 
	}

	void deleteShaders() {
		for (auto&& shader : shaders) {
			shader.deleteShader();
		}
	}

	void deleteProgram() {
		if (isProgramValid) {
			glDeleteProgram(program);
			isProgramValid = false; 
		}
	}

	void setMatrix (std::string uniformName, Matrix4f matrix) {
		useProgram();
		glUniformMatrix4fv(glGetUniformLocation(program, uniformName.c_str())
				, 1, GL_TRUE, matrix.returnFloatArray());
	}

	void setVector (std::string uniformName, Vector4f vec) {
		useProgram();
		glUniform4fv(glGetUniformLocation(program, uniformName.c_str()), 1, vec.array);
	}

	void setVector (std::string uniformName, Vector3f vec) {
		useProgram();
		glUniform3fv(glGetUniformLocation(program, uniformName.c_str()), 1, vec.array);
	}

	void setVector (std::string uniformName, Vector2f vec) {
		useProgram();
		glUniform2fv(glGetUniformLocation(program, uniformName.c_str()), 1, vec.array);
	}

	void setFloat (std::string uniformName, float value) {
		useProgram();
		glUniform1f(glGetUniformLocation(program, uniformName.c_str()), value);
	}

	void setInt (std::string uniformName, int value) {
		useProgram();
		glUniform1i(glGetUniformLocation(program, uniformName.c_str()), value);
	}

	void bindAttributeLocation(std::string name, int index) {
		glBindAttribLocation(program, index, name.c_str());
	}
};


#endif