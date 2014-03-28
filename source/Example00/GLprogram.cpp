#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <iostream>
#include <string>
#include <map>

#include "OVR.h"

#include "GLprogram.h"

#undef new

using namespace std;

GLprogram::GLprogram() 
	: m_vertexShader(0)
	, m_fragmentShader(0)
	, m_program(0)
	, m_wasCalled(false) { }

void GLprogram::open(const string & name) {
	// load shaders only once
	if(!m_wasCalled){
		cout << "open shader " << name << "\n";
		open(name + ".vs", name + ".fs");
		m_wasCalled = true;
	}
}

void GLprogram::open(const string & m_vertexShaderFile, const string & fragmentShaderFile) {
	string source = loadResource(m_vertexShaderFile);
	m_vertexShader = compileShader(GL_VERTEX_SHADER, source);
	source = loadResource(fragmentShaderFile);
	m_fragmentShader = compileShader(GL_FRAGMENT_SHADER, source);
	m_program = linkProgram(m_vertexShader, m_fragmentShader);
	m_attributes.clear();
	GLchar GL_OUTPUT_BUFFER[8192];
	int numVars;
	glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &numVars);
	for (int i = 0; i < numVars; ++i) {
		GLsizei bufSize = 8192;
		GLsizei size; GLenum type;
		glGetActiveAttrib(m_program, i, bufSize, &bufSize, &size, &type, GL_OUTPUT_BUFFER);
		string name = string(GL_OUTPUT_BUFFER, bufSize);
		GLint location = glGetAttribLocation(m_program, name.c_str());
		m_attributes[name] = location;
		cout << "Found attribute " << name << " at location " << location << endl;
	}

	m_uniforms.clear();
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &numVars);
	for (int i = 0; i < numVars; ++i) {
		GLsizei bufSize = 8192;
		GLsizei size;
		GLenum type;
		glGetActiveUniform(m_program, i, bufSize, &bufSize, &size, &type, GL_OUTPUT_BUFFER);
		string name = string(GL_OUTPUT_BUFFER, bufSize);
		GLint location = glGetUniformLocation(m_program, name.c_str());
		m_uniforms[name] = location;
		cout << "Found uniform " << name << " at location " << location << endl;
	}
}

void GLprogram::use() {
	glUseProgram(m_program);
}

void GLprogram::close() {
	if (0 != m_program) {
		glDeleteProgram(m_program);
		m_program = 0;
	}
	if (0 != m_vertexShader) {
		glDeleteShader(m_vertexShader);
	}
	if (0 != m_fragmentShader) {
		glDeleteShader(m_fragmentShader);
	}
}

GLint GLprogram::getUniformLocation(const string & uniform) const {
	auto itr = m_uniforms.find(uniform);
	if (m_uniforms.end() != itr) {
		return itr->second;
	}
	return -1;
}

GLint GLprogram::getAttributeLocation(const string & attribute) const {
	Map::const_iterator itr = m_attributes.find(attribute);
	if (m_attributes.end() != itr) {
		return itr->second;
	}
	return -1;
}

void GLprogram::uniformMat4(const string & uniform, const glm::mat4 & mat) const {
	glUniformMatrix4fv(getUniformLocation(uniform), 1, GL_FALSE, glm::value_ptr(mat));
}
void GLprogram::uniform4f(const string & uniform, float a, float b, float c, float d) const{
	glUniform4f(getUniformLocation(uniform), a, b, c, d);
}
void GLprogram::uniform4f(const string & uniform, const float * fv) const {
	uniform4f(uniform, fv[0], fv[1], fv[2], fv[3]);
}
void GLprogram::uniform2f(const string & uniform, float a, float b) const {
	glUniform2f(getUniformLocation(uniform), a, b);
}
void GLprogram::uniform2f(const string & uniform, const glm::vec2 & vec) const {
	uniform2f(uniform, vec.x, vec.y);
}

string GLprogram::loadResource(const string& in) {	// TODO multi platform
	static HMODULE module = GetModuleHandle(NULL);
	HRSRC res = FindResourceA(module, in.c_str(), "TEXTFILE");
	HGLOBAL mem = LoadResource(module, res);
	DWORD size = SizeofResource(module, res);
	LPVOID data = LockResource(mem);
	string result((const char*)data, size);
	FreeResource(mem);
	return result;
}

// A small class to encapsulate loading of shaders into a GL program
string GLprogram::getProgramLog(GLuint program) {
	string log;
	GLint infoLen = 0;
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);

	if (infoLen > 1) {
		char* infoLog = new char[infoLen];
		glGetProgramInfoLog(program, infoLen, NULL, infoLog);
		log = string(infoLog);
		delete[] infoLog;
	}
	return log;
}

string GLprogram::getShaderLog(GLuint shader) {
	string log;
	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

	if (infoLen > 1) {
		char* infoLog = new char[infoLen];
		glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
		log = string(infoLog);
		delete[] infoLog;
	}
	return log;
}

GLuint GLprogram::compileShader(GLuint type, const string shaderSrc) {
	// Create the shader object
	GLuint shader = glCreateShader(type);
	assert(shader != 0);
	const char * srcPtr = shaderSrc.c_str();
	glShaderSource(shader, 1, &srcPtr, NULL);
	glCompileShader(shader);
	GLint compiled;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (compiled == 0) {
		string errorLog = getShaderLog(shader);
		cerr << errorLog << endl;
	}
	assert(compiled != 0);
	return shader;
}
	
GLuint GLprogram::linkProgram(GLuint m_vertexShader, GLuint fragmentShader) {
	GLuint program = glCreateProgram();
	assert(program != 0);
	glAttachShader(program, m_vertexShader);
	glAttachShader(program, fragmentShader);
	// Link the newProgram
	glLinkProgram(program);
	// Check the link status
	GLint linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if (linked == 0) {
		cerr << getProgramLog(program) << endl;
	}
	assert(linked != 0);
	return program;
}