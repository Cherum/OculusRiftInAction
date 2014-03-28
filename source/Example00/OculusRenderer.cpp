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

#include "CoinConverter.h"
#include "OculusRenderer.h"
#include "GLprogram.h"
#include "DebugHelper.h"

#undef new

using namespace std;
using namespace OVR;
using namespace OVR::Util::Render;

const StereoEye OculusRenderer::s_EYES[2] = { StereoEye_Left, StereoEye_Right };

OculusRenderer::OculusRenderer() 
	: m_renderMode(MONO)
	, m_useTracker(false)
	, m_elapsed(0)
	, m_cubeVertexBuffer(0)
	, m_cubeIndexBuffer(0)
	, m_cubeWireIndexBuffer(0)
	, m_quadVertexBuffer(0)
	, m_quadIndexBuffer(0)
	, m_frameBuffer(0)
	, m_frameBufferTexture(0)
	, m_depthBuffer(0)
	, m_nearclip(0.1f)
	, m_farclip(100.0f) {

	qDebug() << "Example00()";
	initOculus();
}
//
OculusRenderer::~OculusRenderer() {
	m_sensorFusion.AttachToSensor(nullptr);
	m_ovrSensor.Clear();
	OVR::System::Destroy();
}
//
void OculusRenderer::initializeGL(){
	// Since QGLFunctions is bugged, use glew for openGL access
	glewInit();

	setGlValues();
	loadShaders();

	// Use Window Resolution for correct distortion
	resize(m_hmdInfo.HResolution, m_hmdInfo.VResolution);
	move(0,0);
	setWindowTitle("Oculus QGLWidget");
	makeCurrent();

	loadCoinScene();

	m_modelview = glm::lookAt(CAMERA, ORIGIN, UP);
	m_projection = glm::perspective(60.0f, (float)m_hmdInfo.HResolution / (float)m_hmdInfo.VResolution, 0.1f, 100.f);
}

void OculusRenderer::resizeGL(int w, int h){
	qDebug() << "resizeGL";
}
//
void OculusRenderer::paintGL(){
	draw();
	//update();	// TODO debug
}
//
void OculusRenderer::loadCoinScene(){
	qDebug() << "loadCoinScene";

	// Init Coin
	SoDB::init();
	// The m_root node
	m_root = new SoSeparator;
	m_root->ref();

	// It is mandatory to have at least one light for the offscreen renderer
	SoDirectionalLight * light = new SoDirectionalLight;
	m_root->addChild(light);
	light->direction = SbVec3f(0, -0.5, -0.3);

	m_vpRegion.setViewportPixels(0, 0, m_hmdInfo.HResolution, m_hmdInfo.VResolution);
			
	m_perscam = new SoPerspectiveCamera();
	m_root->addChild(m_perscam);

	SoMaterial * greenmaterial = new SoMaterial;
	greenmaterial->diffuseColor.setValue(0, 0.5, 0.5);
	SoCube * cube = new SoCube;
	m_root->addChild(greenmaterial);
	m_root->addChild(cube);
			
	// same as m_projection = glm::perspective(60.0f, 
	//				(float)m_hmdInfo.HResolution / (float)m_hmdInfo.VResolution, 0.1f, 100.f);
	m_perscam->nearDistance = 0.1;
	m_perscam->farDistance = 100.0f;
	m_perscam->heightAngle = glm::radians(60.0f);
	m_perscam->aspectRatio = (float)m_hmdInfo.HResolution / (float)m_hmdInfo.VResolution;
	m_perscam->viewportMapping = SoPerspectiveCamera::LEAVE_ALONE;

	m_sceneManager = new SoSceneManager();
	m_sceneManager->setSceneGraph(m_root);

	m_xMultiplier = 1;
	m_yMultiplier = 1;

	m_coinNearClip = 2.32616;
	m_coinFarClip = 5.79026;
	m_perscam->nearDistance = m_coinNearClip;
	m_perscam->farDistance = m_coinFarClip;
	m_cameraRoot = SbVec3f(0, 0, 4.05821);
}
//
void OculusRenderer::initOculus(){
	// do the master initialization for the Oculus VR SDK
	OVR::System::Init();

	m_sensorFusion.SetGravityEnabled(false);
	m_sensorFusion.SetPredictionEnabled(false);
	m_sensorFusion.SetYawCorrectionEnabled(false);

	m_hmdInfo.HResolution = 1280;
	m_hmdInfo.VResolution = 800;
	m_hmdInfo.HScreenSize = 0.149759993f;
	m_hmdInfo.VScreenSize = 0.0935999975f;
	m_hmdInfo.VScreenCenter = 0.0467999987f;
	m_hmdInfo.EyeToScreenDistance    = 0.0410000011f;
	m_hmdInfo.LensSeparationDistance = 0.0635000020f;
	m_hmdInfo.InterpupillaryDistance = 0.0640000030f;
	m_hmdInfo.DistortionK[0] = 1.00000000f;
	m_hmdInfo.DistortionK[1] = 0.219999999f;
	m_hmdInfo.DistortionK[2] = 0.239999995f;
	m_hmdInfo.DistortionK[3] = 0.000000000f;
	m_hmdInfo.ChromaAbCorrection[0] = 0.995999992f;
	m_hmdInfo.ChromaAbCorrection[1] = -0.00400000019f;
	m_hmdInfo.ChromaAbCorrection[2] = 1.01400006f;
	m_hmdInfo.ChromaAbCorrection[3] = 0.000000000f;
	m_hmdInfo.DesktopX = 0;
	m_hmdInfo.DesktopY = 0;


	///////////////////////////////////////////////////////////////////////////
	// Initialize Oculus VR SDK and hardware
	Ptr<DeviceManager> ovrManager = *DeviceManager::Create();
	if (ovrManager) {
		m_ovrSensor = *ovrManager->EnumerateDevices<SensorDevice>().CreateDevice();
		if (m_ovrSensor) {
			m_useTracker = true;
			m_sensorFusion.AttachToSensor(m_ovrSensor);
		}
		Ptr<HMDDevice> ovrHmd = *ovrManager->EnumerateDevices<HMDDevice>().CreateDevice();
		if (ovrHmd) {
			ovrHmd->GetDeviceInfo(&m_hmdInfo);
		}
		// The HMDInfo structure contains everything we need for now, so no
		// need to keep the device handle around
		ovrHmd.Clear();
	}
	// The device manager is reference counted and will be released automatically
	// when our sensorObject is destroyed.
	ovrManager.Clear();
	m_stereoConfig.SetHMDInfo(m_hmdInfo);
	m_stereoConfig.SetStereoMode(StereoMode::Stereo_None);
}
// 
void OculusRenderer::setGlValues(){
	// Enable the zbuffer test
	glEnable(GL_DEPTH_TEST);
	glLineWidth(2.0f);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	glGenBuffers(1, &m_cubeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,
			sizeof(GLfloat) * VERT_COUNT * VERTICES_PER_TRIANGLE, CUBE_VERTEX_DATA, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_cubeIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(GLuint) * FACE_COUNT * TRIANGLES_PER_FACE * VERTICES_PER_TRIANGLE,
			CUBE_INDICES, GL_STATIC_DRAW);

	glGenBuffers(1, &m_cubeWireIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			sizeof(GLuint) * EDGE_COUNT * VERTICES_PER_EDGE,
			CUBE_WIRE_INDICES, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &m_quadVertexBuffer);
	glGenBuffers(1, &m_quadIndexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadIndexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, QUAD_INDICES, GL_STATIC_DRAW);


	m_eyeWidth = m_hmdInfo.HResolution / 2;
	m_eyeHeight = m_hmdInfo.VResolution;
	m_fboWidth = m_eyeWidth * FRAMEBUFFER_OBJECT_SCALE;
	m_fboHeight = m_eyeHeight * FRAMEBUFFER_OBJECT_SCALE;

	glGenFramebuffers(1, &m_frameBuffer);
	glGenFramebuffers(1, &m_frameBufferTest);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_frameBufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_frameBufferTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Allocate space for the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fboWidth, m_fboHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_frameBufferTexture, 0);
	glGenRenderbuffers(1, &m_depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, m_depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, m_fboWidth, m_fboHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthBuffer);
	glEnable(GL_TEXTURE_2D);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// Create the rendering shaders
void OculusRenderer::loadShaders(){
	m_renderProgram.open("Simple");
	m_textureProgram.open("Texture");
	m_distortProgram.open("Distort");
}
//
void OculusRenderer::keyPressEvent( QKeyEvent * event ){
	switch (event->key()){
		case Qt::Key_P:
			m_renderMode = static_cast<Mode>((m_renderMode + 1) % 3);
			if (m_renderMode == MONO) {

				m_projection = glm::perspective(60.0f,
					(float) m_hmdInfo.HResolution / (float) m_hmdInfo.VResolution, m_nearclip, m_farclip);

				////
				m_perscam->heightAngle = glm::radians(60.0f);
				m_perscam->aspectRatio = (float) m_hmdInfo.HResolution / (float) m_hmdInfo.VResolution;
				m_stereoConfig.SetStereoMode(StereoMode::Stereo_None);
			} else if (m_renderMode == STEREO) {

				m_projection = glm::perspective(60.0f,
					(float) m_hmdInfo.HResolution / 2.0f / (float) m_hmdInfo.VResolution, m_nearclip, m_farclip);

				////
				m_perscam->heightAngle = glm::radians(60.0f);
				m_perscam->aspectRatio = (float)m_hmdInfo.HResolution / 2.0f / (float)m_hmdInfo.VResolution;
				m_stereoConfig.SetStereoMode(StereoMode::Stereo_LeftRight_Multipass);
			} else if (m_renderMode == STEREO_DISTORT) {

				m_projection = glm::perspective(m_stereoConfig.GetYFOVDegrees(),
					(float) m_hmdInfo.HResolution / 2.0f / (float) m_hmdInfo.VResolution, m_nearclip, m_farclip);

				////
				m_perscam->heightAngle = m_stereoConfig.GetYFOVRadians();
				m_perscam->aspectRatio = (float) m_hmdInfo.HResolution / 2.0f / (float) m_hmdInfo.VResolution;
				m_stereoConfig.SetStereoMode(StereoMode::Stereo_LeftRight_Multipass);
			}
			break;
		case Qt::Key_Up:
			m_xMultiplier += 0.5;
			qDebug() << "m_xMultiplier+" << m_xMultiplier;
			break;
		case Qt::Key_Down:
			m_xMultiplier -= 0.5;
			qDebug() << "m_xMultiplier-" << m_xMultiplier;
			break;
		case Qt::Key_Left:
			m_yMultiplier += 0.5;
			qDebug() << "m_yMultiplier+" << m_yMultiplier;
			break;
		case Qt::Key_Right:
			m_yMultiplier -= 0.5;
			qDebug() << "m_yMultiplier-" << m_yMultiplier;
			break;
	}
	QWidget ::keyPressEvent(event);
}
//
void OculusRenderer::update() {
	long now = millis();
	if (m_useTracker) {
		// For some reason building the quaternion directly from the OVR
		// x,y,z,w values does not work.  So instead we convert it into
		// euler angles and construct our glm::quaternion from those

		// Fetch the pitch roll and yaw out of the m_sensorFusion device
		glm::vec3 eulerAngles;
		m_sensorFusion.GetOrientation().GetEulerAngles<Axis_X, Axis_Y, Axis_Z, Rotate_CW, Handed_R>(
			&eulerAngles.x, &eulerAngles.y, &eulerAngles.z);

		// Not convert it into a GLM quaternion.
		glm::quat orientation = glm::quat(eulerAngles);

		// Most applications want take a basic camera postion and apply the
		// orientation transform to it in this way:
		//m_modelview = glm::mat4_cast(orientation) * glm::lookAt(CAMERA, ORIGIN, UP);

		// However for this demonstration we want the cube to remain
		// centered in the viewport, and orbit our view around it.  This
		// serves two purposes.
		//
		// First, it's not possible to see a blank screen in the event
		// the HMD is oriented to point away from the origin of the scene.
		//
		// Second, a scene that has no points of reference other than a
		// single small object can be disorienting, leaving the user
		// feeling lost in a void.  Having a fixed object in the center
		// of the screen that you appear to be moving around should
		// provide less immersion, which in this instance is better
		m_modelview = glm::lookAt(CAMERA, ORIGIN, UP) * glm::mat4_cast(orientation);
	} else {
		// In the absence of head tracker information, we want to slowly
		// rotate the cube so that the animation of the scene is apparent
		static const float Y_ROTATION_RATE = 0.01f;
		static const float Z_ROTATION_RATE = 0.05f;
		m_modelview = glm::lookAt(CAMERA, ORIGIN, UP);
		m_modelview = glm::rotate(m_modelview, m_elapsed * Y_ROTATION_RATE, Y_AXIS);
		m_modelview = glm::rotate(m_modelview, m_elapsed * Z_ROTATION_RATE, Z_AXIS);
	}
	m_elapsed = now;

	QGLWidget::update();
}
//
void OculusRenderer::draw() {
	if (m_renderMode == MONO) {
		// If we're not working stereo, we're just going to render the
		// scene once, from a single position, directly to the back buffer
		m_vpRegion.setViewportPixels(0, 0, m_hmdInfo.HResolution, m_hmdInfo.VResolution);

		glViewport(0, 0, m_hmdInfo.HResolution, m_hmdInfo.VResolution);
		renderCubeScene(glm::vec3(), glm::vec3(), m_stereoConfig.GetEyeRenderParams(StereoEye_Center)); 
		renderCoinScene(glm::vec3(), glm::vec3(), m_stereoConfig.GetEyeRenderParams(StereoEye_Center));
	} else {
		// If we get here, we're rendering in stereo, so we have to render our output twice
		// We have to explicitly clear the screen here.  the Clear command doesn't object the viewport
		// and the clear command inside renderScene will only target the active framebuffer object.
		glClearColor(0, 1, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		for (int i = 0; i < 2; ++i) {
			StereoEye eye = s_EYES[i];
			glBindTexture(GL_TEXTURE_2D, 0);
			// Compute the m_modelview and m_projection matrices for the rendered scene based on the eye and
			// whether or not we're doing side by side or rift rendering
			glm::vec3 eyeProjectionOffset;
			if (m_renderMode == STEREO_DISTORT) {
				eyeProjectionOffset = glm::vec3(-m_stereoConfig.GetProjectionCenterOffset() / 2.0f, 0, 0);
			}
			glm::vec3 eyeModelviewOffset = glm::vec3(-m_stereoConfig.GetIPD() / 2.0f, 0, 0);
			if (eye == StereoEye_Left) {
				eyeModelviewOffset *= -1;
				eyeProjectionOffset *= -1;
			}
				
			m_vpRegion.setViewportPixels(0, 0, m_fboWidth, m_fboHeight);

			glViewport(0, 0, m_fboWidth, m_fboHeight);
			glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
			renderCubeScene(eyeProjectionOffset, eyeModelviewOffset, m_stereoConfig.GetEyeRenderParams(eye));
			renderCoinScene(eyeProjectionOffset, eyeModelviewOffset, m_stereoConfig.GetEyeRenderParams(eye));
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			// Setup the viewport for the eye to which we're rendering
			glViewport(1 + (eye == StereoEye_Left ? 0 : m_eyeWidth), 1, m_eyeWidth - 2, m_eyeHeight - 2);
			GLprogram & program = (m_renderMode == STEREO_DISTORT) ? m_distortProgram : m_textureProgram;
			program.use();
			GLint positionLocation = program.getAttributeLocation("Position");
			assert(positionLocation > -1);
			GLint texCoordLocation = program.getAttributeLocation("TexCoord");
			assert(texCoordLocation > -1);

			float texL = 0, texR = 1, texT = 1, texB = 0;
			if (m_renderMode == STEREO_DISTORT) {
				// Pysical width of the viewport
				static float eyeScreenWidth = m_hmdInfo.HScreenSize / 2.0f;
				// The viewport goes from -1,1.  We want to get the offset
				// of the lens from the center of the viewport, so we only
				// want to look at the distance from 0, 1, so we divide in
				// half again
				static float halfEyeScreenWidth = eyeScreenWidth / 2.0f;

				// The distance from the center of the display panel (NOT
				// the center of the viewport) to the lens axis
				static float lensDistanceFromScreenCenter = m_hmdInfo.LensSeparationDistance / 2.0f;

				// Now we we want to turn the measurement from
				// millimeters into the range 0, 1
				static float lensDistanceFromViewportEdge = lensDistanceFromScreenCenter / halfEyeScreenWidth;

				// Finally, we want the distnace from the center, not the
				// distance from the edge, so subtract the value from 1
				static float lensOffset = 1.0f - lensDistanceFromViewportEdge;
				static glm::vec2 aspect(1.0, (float)m_eyeWidth / (float)m_eyeHeight);

				glm::vec2 lensCenter(lensOffset, 0);

				// Texture coordinates need to be in lens-space for the
				// distort shader
				texL = -1 - lensOffset;
				texR = 1 - lensOffset;
				texT = 1 / aspect.y;
				texB = -1 / aspect.y;
				// Flip the values for the right eye
				if (eye != StereoEye_Left) {
					swap(texL, texR);
					texL *= -1;
					texR *= -1;
					lensCenter *= -1;
				}

				static glm::vec2 distortionScale(1.0f / m_stereoConfig.GetDistortionScale(),
					1.0f / m_stereoConfig.GetDistortionScale());
				program.uniform2f("LensCenter", lensCenter);
				program.uniform2f("Aspect", aspect);
				program.uniform2f("DistortionScale", distortionScale);
				program.uniform4f("K", m_hmdInfo.DistortionK);
			}

			// Vertices zeichnen
			const GLfloat quadVertices[] = {
				-1, -1, texL, texB,
					1, -1, texR, texB,
					1,  1, texR, texT,
				-1,  1, texL, texT,
			};

			glBindTexture(GL_TEXTURE_2D, m_frameBufferTexture);
			glBindBuffer(GL_ARRAY_BUFFER, m_quadVertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 2 * 4, quadVertices, GL_DYNAMIC_DRAW);

			int stride = sizeof(GLfloat) * 2 * 2;
			glEnableVertexAttribArray(positionLocation);
			glVertexAttribPointer(positionLocation, 2, GL_FLOAT, GL_FALSE, stride, 0);
			glEnableVertexAttribArray(texCoordLocation);
			glVertexAttribPointer(texCoordLocation, 2, GL_FLOAT, GL_FALSE, stride, (GLvoid*)(sizeof(GLfloat) * 2));

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_quadIndexBuffer);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (GLvoid*)0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		} // for
	} // if
}
//
void OculusRenderer::renderCubeScene(const glm::vec3 & m_projectionOffset, const glm::vec3 & m_modelviewOffset, const StereoEyeParams eyeParam) {
	glm::mat4 sceneProjection = glm::translate(glm::mat4(), m_projectionOffset) * m_projection;
	glm::mat4 sceneModelview = glm::translate(glm::mat4(), m_modelviewOffset) * m_modelview;

	// Clear the buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Configure the GL pipeline for rendering our geometry
	m_renderProgram.use();

	// Load the m_projection and m_modelview matrices into the program
	m_renderProgram.uniformMat4("Projection", sceneProjection);
	m_renderProgram.uniformMat4("ModelView", sceneModelview);
		
	// Load up our cube geometry (vertices and indices)
	glBindBuffer(GL_ARRAY_BUFFER, m_cubeVertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeIndexBuffer);

	// Bind the vertex data to the program
	GLint positionLocation = m_renderProgram.getAttributeLocation("Position");
	GLint colorLocation = m_renderProgram.getUniformLocation("Color");
		
	glEnableVertexAttribArray(positionLocation);
	glVertexAttribPointer(positionLocation, 3, GL_FLOAT, GL_FALSE, 12, (GLvoid*)0);
	
	// Draw the cube faces, two calls for each face in order to set the color and then draw the geometry
	for (uintptr_t i = 0; i < FACE_COUNT; ++i) {
		m_renderProgram.uniform4f("Color", CUBE_FACE_COLORS + (i * 4));
		glDrawElements(GL_TRIANGLES, TRIANGLES_PER_FACE * VERTICES_PER_TRIANGLE, GL_UNSIGNED_INT, (void*)(i * 6 * 4));
	}

	// Now scale the m_modelview matrix slightly, so we can draw the cube outline
	//glm::mat4 scaledCamera = glm::scale(sceneModelview, glm::vec3(1.01f));
	//m_renderProgram.uniformMat4("ModelView", scaledCamera);

	// Drawing a white wireframe around the cube
	//glUniform4f(colorLocation, 1, 1, 1, 1);

	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_cubeWireIndexBuffer);
	glDrawElements(GL_LINES, EDGE_COUNT * VERTICES_PER_EDGE, GL_UNSIGNED_INT, (void*)0);*/
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}
//
void OculusRenderer::renderCoinScene(const glm::vec3 & m_projectionOffset, const glm::vec3 & m_modelviewOffset, const StereoEyeParams eyeParam) {
	glm::mat4 sceneProjection = glm::translate(glm::mat4(), m_projectionOffset) * m_projection;
	glm::mat4 sceneModelview = glm::translate(glm::mat4(), m_modelviewOffset) * m_modelview;
	glm::mat4 sceneProjModelProduct = sceneProjection * sceneModelview;	// experimental

	///////////////////////////////////////////////////////////////////////////////////////////
	//m_perscam->viewAll(m_root, m_vpRegion);
	// sets m_nearclip 2.32616 m_farclip 5.79026 position ( 0 / 0 / 4.05821 )

	m_sceneManager->setViewportRegion(m_vpRegion);
	m_sceneManager->render(false, false);

	SbMatrix SbSceneProjModelView = CoinConverter::toSbMatrix(sceneProjModelProduct);
	//SbMatrix SbSceneProjModelView = toSbMatrix(coinSceneProjModelProduct);

	SbVec3f translation;
	SbRotation rotation;
	SbVec3f scaleFactor;
	SbRotation scaleOrientation;
	SbSceneProjModelView.getTransform(translation, rotation, scaleFactor, scaleOrientation);

	m_perscam->position = SbVec3f(
		m_cameraRoot.getValue()[0] + translation.getValue()[0] * m_xMultiplier, 
		m_cameraRoot.getValue()[1], 
		m_cameraRoot.getValue()[2]
	);
		
	//printMatrix4(SbSceneProjModelView);
	//qDebug() << "translation[0]" << translation.getValue()[0];

	//m_perscam->position = translation;
	//m_perscam->position = SbVec3f(translation.getValue()[0] * m_xMultiplier, translation.getValue()[1], translation.getValue()[2]);
	m_perscam->orientation = SbRotation(-rotation.getValue()[0], -rotation.getValue()[1], rotation.getValue()[2], -rotation.getValue()[3]);
}
	
QQuaternion OculusRenderer::getOrientation() const{
	// Create identity quaternion
	QQuaternion osgQuat(0.0f, 0.0f, 0.0f, 1.0f);

	if (m_sensorFusion.IsAttachedToSensor()) {
		OVR::Quatf quat = m_sensorFusion.GetOrientation();
		osgQuat = QQuaternion(quat.w, quat.x, quat.y, quat.z);
	}

	return osgQuat;
}

long OculusRenderer::millis() {	// TODO make platform independent
	static long start = GetTickCount();
	return GetTickCount() - start;
}