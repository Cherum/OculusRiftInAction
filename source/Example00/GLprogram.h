#pragma once

#ifndef GLprogram_h__
#define GLprogram_h__

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <string>
#include <map>

using namespace std;

class GLprogram {
public:
	GLprogram();

	bool m_wasCalled;
		
	void open(const string & name);
	void open(const string & m_vertexShaderFile, const string & fragmentShaderFile);
	void use();
	void close();

	GLint getUniformLocation(const string & uniform) const;
	GLint getAttributeLocation(const string & attribute) const;

	void uniformMat4(const string & uniform, const glm::mat4 & mat) const;
	void uniform4f(const string & uniform, float a, float b, float c, float d) const;
	void uniform4f(const string & uniform, const float * fv) const;
	void uniform2f(const string & uniform, float a, float b) const;
	void uniform2f(const string & uniform, const glm::vec2 & vec) const;

protected:
	GLuint m_vertexShader;
	GLuint m_fragmentShader;
	GLuint m_program;

	typedef map<string, GLint> Map;
	Map m_attributes;
	Map m_uniforms;

	static string loadResource(const string& in);

	static string getProgramLog(GLuint program);
	static string getShaderLog(GLuint shader);
	static GLuint compileShader(GLuint type, const string shaderSrc);
	static GLuint linkProgram(GLuint m_vertexShader, GLuint fragmentShader);
};

#endif // GLprogram_h__