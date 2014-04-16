#include <GL/glew.h>

#include <QApplication>
#include <QGLWidget>
#include <qdebug.h>

#include "main.h"

using namespace OVR;
using namespace OVR::Util::Render;

int main(int argc, char ** argv) {
	QApplication app(argc, argv);

	OculusRenderer renderer = OculusRenderer();

	// widget
	renderer.show();
	app.exec();
}

OculusRenderer::OculusRenderer(){
	initOculus();
}
//
void OculusRenderer::initOculus(){
	// do the master initialization for the Oculus VR SDK
	OVR::System::Init();

	///////////////////////////////////////////////////////////////////////////
	// Initialize Oculus VR SDK and hardware
	Ptr<DeviceManager> ovrManager = *DeviceManager::Create();
	if (ovrManager) {
		m_ovrSensor = *ovrManager->EnumerateDevices<SensorDevice>().CreateDevice();
		if (m_ovrSensor) {
			bool wasAttached = m_sensorFusion.AttachToSensor(m_ovrSensor);
			qDebug() << "Sensor attached:" << wasAttached;
			qDebug() << "sensorFusion11" << m_sensorFusion.IsAttachedToSensor();
		}
		else {
			qDebug() << "Sensor NOT attached!";
		}
	}
}