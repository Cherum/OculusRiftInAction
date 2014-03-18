#pragma once

#ifndef DebugHelper_h__
#define DebugHelper_h__

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <qdebug.h>

#include <Inventor\SbRotation.h>
#include <Inventor\SbVec3f.h>
#include <Inventor\SbMatrix.h>
#include <Inventor\fields\SoSFRotation.h>
#include <Inventor\fields\SoSFVec3f.h>

#include "OVR.h"

class DebugHelper {
public:
	static void checkGlError() {
		GLenum error = glGetError();
		if (error != GL_NO_ERROR) {
			switch(error){
				case GL_INVALID_ENUM:
					qDebug() << "OpenGL Error: GL_INVALID_ENUM";
					break;
				case GL_INVALID_VALUE:
					qDebug() << "OpenGL Error: GL_INVALID_VALUE";
					break;
				case GL_INVALID_OPERATION:
					qDebug() << "OpenGL Error: GL_INVALID_OPERATION";
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					qDebug() << "OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION";
					break;
				case GL_OUT_OF_MEMORY:
					qDebug() << "OpenGL Error: GL_OUT_OF_MEMORY";
					break;
				case GL_STACK_UNDERFLOW:
					qDebug() << "OpenGL Error: GL_STACK_UNDERFLOW";
					break;
				case GL_STACK_OVERFLOW:
					qDebug() << "OpenGL Error GL_STACK_OVERFLOW";
					break;
			}
		}
		//assert(error == 0);
	}

	static void printVector3(glm::vec3 vec){
		qDebug() << "(" << vec.x << "/" << vec.y << "/" << vec.z << ")";
	}
	/*static void printVector3(SoSFVec3f vec){
		qDebug() << "(" << vec.getValue<()[0] << "/" << vec.getValue()[1] << "/" << vec.getValue()[2] << ")";
	}*/
	static void printVector3(SbVec3f vec){
		qDebug() << "(" << vec.getValue()[0] << "/" << vec.getValue()[1] << "/" << vec.getValue()[2] << ")";
	}

	static void printVector4(glm::vec4 vec){
		qDebug() << "(" << vec.x << "/" << vec.y << "/" << vec.z << "/" << vec.w << ")";
	}

	static void printMatrix4(glm::mat4 mat){
		qDebug() << "(" << mat[0][0] << "," << mat[1][0] << "," << mat[2][0] << "," << mat[3][0] << ")";
		qDebug() << "(" << mat[0][1] << "," << mat[1][1] << "," << mat[2][1] << "," << mat[3][1] << ")";
		qDebug() << "(" << mat[0][2] << "," << mat[1][2] << "," << mat[2][2] << "," << mat[3][2] << ")";
		qDebug() << "(" << mat[0][3] << "," << mat[1][3] << "," << mat[2][3] << "," << mat[3][3] << ")";
	}
	static void printMatrix4(OVR::Matrix4f mat){
		qDebug() << "(" << mat.M[0][0] << "," << mat.M[1][0] << "," << mat.M[2][0] << "," << mat.M[3][0] << ")";
		qDebug() << "(" << mat.M[0][1] << "," << mat.M[1][1] << "," << mat.M[2][1] << "," << mat.M[3][1] << ")";
		qDebug() << "(" << mat.M[0][2] << "," << mat.M[1][2] << "," << mat.M[2][2] << "," << mat.M[3][2] << ")";
		qDebug() << "(" << mat.M[0][3] << "," << mat.M[1][3] << "," << mat.M[2][3] << "," << mat.M[3][3] << ")";
	}
	static void printMatrix4(SbMatrix mat){
		qDebug() << "(" << mat.getValue()[0][0] << "," << mat.getValue()[1][0] << "," << mat.getValue()[2][0] << "," << mat.getValue()[3][0] << ")";
		qDebug() << "(" << mat.getValue()[0][1] << "," << mat.getValue()[1][1] << "," << mat.getValue()[2][1] << "," << mat.getValue()[3][1] << ")";
		qDebug() << "(" << mat.getValue()[0][2] << "," << mat.getValue()[1][2] << "," << mat.getValue()[2][2] << "," << mat.getValue()[3][2] << ")";
		qDebug() << "(" << mat.getValue()[0][3] << "," << mat.getValue()[1][3] << "," << mat.getValue()[2][3] << "," << mat.getValue()[3][3] << ")";
	}

	static void printRotation(SoSFRotation rot){
		SbRotation r = rot.getValue();
		qDebug() << "rot (" << r.getValue()[0] << "," << r.getValue()[1] << "," << r.getValue()[2] << "," << r.getValue()[3] << ")";
	}
	static void printRotation(SbRotation r){
		qDebug() << "rot (" << r.getValue()[0] << "," << r.getValue()[1] << "," << r.getValue()[2] << "," << r.getValue()[3] << ")";
	}
};
#endif // DebugHelper_h__