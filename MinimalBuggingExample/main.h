#include <GL/glew.h>

#include <QApplication>
#include <QGLWidget>

#include "OVR.h"

using namespace OVR;
using namespace OVR::Util::Render;

class OculusRenderer : public QGLWidget {
public:
	OculusRenderer();

private:
	Ptr<SensorDevice> m_ovrSensor;
	SensorFusion m_sensorFusion;

	void initOculus();
};