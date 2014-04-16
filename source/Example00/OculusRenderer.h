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
#include "CubeValues.h"

using namespace std;
using namespace OVR;
using namespace OVR::Util::Render;

class OculusRenderer : public QGLWidget {
public:
	OculusRenderer();
	virtual ~OculusRenderer();

	virtual void keyPressEvent( QKeyEvent * event );

	virtual void update();
	virtual void draw();
	virtual void renderCubeScene(const glm::vec3 & m_projectionOffset, const glm::vec3 & m_modelviewOffset, const StereoEyeParams eyeParam);
	virtual void renderCoinScene(const glm::vec3 & m_projectionOffset, const glm::vec3 & m_modelviewOffset, const StereoEyeParams eyeParam);
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

	void initializeGL();
	void paintGL();

	void setGlValues();
	// Create the rendering shaders
	void loadShaders();
	void initOculus();
	void loadCoinScene();

	long millis();

	QQuaternion getOrientation() const;
};

#endif // OculusRenderer_h__