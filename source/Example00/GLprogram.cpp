#include <GL/glew.h>
//#include <sstream>
//#include <fstream>

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
	: vertexShader(0)
	, fragmentShader(0)
	, program(0)
	, wasCalled(false) { }

void GLprogram::open(const string & name) {
	// load shaders only once
	if(!wasCalled){
		cout << "open shader " << name << "\n";
		open(name + ".vs", name + ".fs");
		wasCalled = true;
	}
}

void GLprogram::open(const string & vertexShaderFile, const string & fragmentShaderFile) {
	string source = loadResource(vertexShaderFile);
	vertexShader = compileShader(GL_VERTEX_SHADER, source);
	source = loadResource(fragmentShaderFile);
	fragmentShader = compileShader(GL_FRAGMENT_SHADER, source);
	program = linkProgram(vertexShader, fragmentShader);
	attributes.clear();
	GLchar GL_OUTPUT_BUFFER[8192];
	int numVars;
	glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &numVars);
	for (int i = 0; i < numVars; ++i) {
		GLsizei bufSize = 8192;
		GLsizei size; GLenum type;
		glGetActiveAttrib(program, i, bufSize, &bufSize, &size, &type, GL_OUTPUT_BUFFER);
		string name = string(GL_OUTPUT_BUFFER, bufSize);
		GLint location = glGetAttribLocation(program, name.c_str());
		attributes[name] = location;
		cout << "Found attribute " << name << " at location " << location << endl;
	}

	uniforms.clear();
	glGetProgramiv(program, GL_ACTIVE_UNIFORMS, &numVars);
	for (int i = 0; i < numVars; ++i) {
		GLsizei bufSize = 8192;
		GLsizei size;
		GLenum type;
		glGetActiveUniform(program, i, bufSize, &bufSize, &size, &type, GL_OUTPUT_BUFFER);
		string name = string(GL_OUTPUT_BUFFER, bufSize);
		GLint location = glGetUniformLocation(program, name.c_str());
		uniforms[name] = location;
		cout << "Found uniform " << name << " at location " << location << endl;
	}
}

void GLprogram::use() {
	glUseProgram(program);
}

void GLprogram::close() {
	if (0 != program) {
		glDeleteProgram(program);
		program = 0;
	}
	if (0 != vertexShader) {
		glDeleteShader(vertexShader);
	}
	if (0 != fragmentShader) {
		glDeleteShader(fragmentShader);
	}
}

GLint GLprogram::getUniformLocation(const string & uniform) const {
	auto itr = uniforms.find(uniform);
	if (uniforms.end() != itr) {
		return itr->second;
	}
	return -1;
}

GLint GLprogram::getAttributeLocation(const string & attribute) const {
	Map::const_iterator itr = attributes.find(attribute);
	if (attributes.end() != itr) {
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
	
GLuint GLprogram::linkProgram(GLuint vertexShader, GLuint fragmentShader) {
	GLuint program = glCreateProgram();
	assert(program != 0);
	glAttachShader(program, vertexShader);
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