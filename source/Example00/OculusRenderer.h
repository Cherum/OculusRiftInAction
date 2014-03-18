#pragma once

#ifndef OculusRenderer_h__
#define OculusRenderer_h__

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoSceneTexture2.h>
#include <Inventor/nodes/SoCube.h>
#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoSphere.h>
#include <Inventor/nodes/SoCoordinate3.h>
#include <Inventor/nodes/SoComplexity.h>
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoTexture2.h>
#include <Inventor/nodes/SoTextureCoordinate2.h>
#include <Inventor/nodes/SoShaderProgram.h>
#include <Inventor/nodes/SoFragmentShader.h>
#include <Inventor/nodes/SoDirectionalLight.h>
#include <Inventor/nodes/SoVertexShader.h>
#include <Inventor/nodes/SoShaderObject.h>
#include <Inventor/nodes/SoTriangleStripSet.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoTransform.h>
#include <Inventor/nodes/SoShaderParameter.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor\fields\SoSFVec3f.h>
#include <Inventor\SoSceneManager.h>
#include <Inventor/SoOffscreenRenderer.h>
#include <Inventor\actions\SoGLRenderAction.h>

#include <QApplication>
#include <QKeyEvent>

#include <QGLShaderProgram>
#include <QGLShader>
#include <QGLWidget>

#include "OVR.h"

#include "GLprogram.h"

using namespace std;
using namespace OVR;
using namespace OVR::Util::Render;

// Some defines to make calculations below more transparent
#define TRIANGLES_PER_FACE 2
#define VERTICES_PER_TRIANGLE 3
#define VERTICES_PER_EDGE 2
#define FLOATS_PER_VERTEX 3

// Cube geometry
#define VERT_COUNT 8
#define FACE_COUNT 6
#define EDGE_COUNT 12

#define CUBE_SIZE 0.4f
#define CUBE_P (CUBE_SIZE / 2.0f)
#define CUBE_N (-1.0f * CUBE_P)

#define ON 1.0
#define PQ 0.25

#define RED 1, 0, 0
#define GREEN 0, 1, 0
#define BLUE 0, 0, 1
#define YELLOW 1, 1, 0
#define CYAN 0, 1, 1
#define MAGENTA 1, 0, 1

// How big do we want our renderbuffer
#define FRAMEBUFFER_OBJECT_SCALE 3

const glm::vec3 X_AXIS = glm::vec3(1.0f, 0.0f, 0.0f);
const glm::vec3 Y_AXIS = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Z_AXIS = glm::vec3(0.0f, 0.0f, 1.0f);        // Animate the cube
const glm::vec3 CAMERA = glm::vec3(0.0f, 0.0f, 0.8f);
const glm::vec3 ORIGIN = glm::vec3(0.0f, 0.0f, 0.0f);
const glm::vec3 UP = Y_AXIS;

// Vertices for a unit cube centered at the origin
const GLfloat CUBE_VERTEX_DATA[VERT_COUNT * FLOATS_PER_VERTEX] = {
	CUBE_N, CUBE_N, CUBE_N, // Vertex 0 position
	CUBE_P, CUBE_N, CUBE_N, // Vertex 1 position
	CUBE_P, CUBE_P, CUBE_N, // Vertex 2 position
	CUBE_N, CUBE_P, CUBE_N, // Vertex 3 position
	CUBE_N, CUBE_N, CUBE_P, // Vertex 4 position
	CUBE_P, CUBE_N, CUBE_P, // Vertex 5 position
	CUBE_P, CUBE_P, CUBE_P, // Vertex 6 position
	CUBE_N, CUBE_P, CUBE_P, // Vertex 7 position
};


const GLfloat CUBE_FACE_COLORS[] = {
	RED, 1,
	GREEN, 1,
	BLUE, 1,
	YELLOW, 1,
	CYAN, 1,
	MAGENTA, 1,
};

// 6 sides * 2 triangles * 3 vertices
const unsigned int CUBE_INDICES[FACE_COUNT * TRIANGLES_PER_FACE * VERTICES_PER_TRIANGLE ] = {
   0, 4, 5, 0, 5, 1, // Face 0
   1, 5, 6, 1, 6, 2, // Face 1
   2, 6, 7, 2, 7, 3, // Face 2
   3, 7, 4, 3, 4, 0, // Face 3
   4, 7, 6, 4, 6, 5, // Face 4
   3, 0, 1, 3, 1, 2  // Face 5
};

//
const unsigned int CUBE_WIRE_INDICES[EDGE_COUNT * VERTICES_PER_EDGE ] = {
   0, 1, 1, 2, 2, 3, 3, 0, // square
   4, 5, 5, 6, 6, 7, 7, 4, // facing square
   0, 4, 1, 5, 2, 6, 3, 7, // transverse lines
};

const GLfloat QUAD_VERTICES[] = {
	-1, -1, 0, 0,
	 1, -1, 1, 0,
	 1,  1, 1, 1,
	-1,  1, 0, 1,
};

const GLuint QUAD_INDICES[] = {
   2, 0, 3, 0, 1, 2,
};

class OculusRenderer : public QGLWidget {
public:
	OculusRenderer();
	virtual ~OculusRenderer();

	void initOpenGL();
	// Create the rendering shaders
	void loadShaders();

	void keyPressEvent( QKeyEvent * event );

	virtual void update();
	virtual void draw();
	virtual void renderScene(const glm::vec3 & m_projectionOffset, const glm::vec3 & m_modelviewOffset, const StereoEyeParams eyeParam);
	virtual void renderCoinScene(const glm::vec3 & m_projectionOffset, const glm::vec3 & m_modelviewOffset, const StereoEyeParams eyeParam);
	
	QQuaternion getOrientation() const;

protected:
	const static enum Mode {
		MONO, STEREO, STEREO_DISTORT
	};
	const static StereoEye s_EYES[2];

	// Provides the resolution and location of the Rift
	HMDInfo m_hmdInfo;
	// Calculated width and height of the per-eye rendering area used
	int m_eyeWidth, m_eyeHeight;
	// Calculated width and height of the frame buffer object used to contain
	// intermediate results for the multipass render
	int m_fboWidth, m_fboHeight;

	Ptr<SensorDevice> m_ovrSensor;
	SensorFusion m_sensorFusion;
	StereoConfig m_stereoConfig;

	Mode m_renderMode;
	bool m_useTracker;
	long m_elapsed;

	GLuint m_cubeVertexBuffer;
	GLuint m_cubeIndexBuffer;
	GLuint m_cubeWireIndexBuffer;

	GLuint m_quadVertexBuffer;
	GLuint m_quadIndexBuffer;

	GLprogram m_renderProgram;
	GLprogram m_textureProgram;
	GLprogram m_distortProgram;

	GLuint m_frameBuffer;
	GLuint m_frameBufferTest;
	GLuint m_frameBufferTexture;
	GLuint m_depthBuffer;

	// additions by Alexander
	SoSeparator *m_root;
	SoPerspectiveCamera *m_perscam;
	SoSceneManager *m_sceneManager;
	SbViewportRegion m_vpRegion;
	

	float m_xMultiplier;
	float m_yMultiplier;

	float m_nearclip;
	float m_farclip;

	float m_coinNearClip;
	float m_coinFarClip;

	SbVec3f m_cameraRoot;

	glm::mat4 m_projection;
	glm::mat4 m_modelview;

	glm::mat4 m_coinProjection;
	glm::mat4 m_coinModelview;

	void initOculus();
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	void loadCoinScene();

	long millis();
};

#endif // OculusRenderer_h__