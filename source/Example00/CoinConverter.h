#pragma once

#ifndef CoinConverter_h__
#define CoinConverter_h__

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include <Inventor\SbRotation.h>
#include <Inventor\SbMatrix.h>
#include <Inventor\SbVec.h>
#include <qvector3d.h>
#include <qquaternion.h>

#include "OVR.h"

using namespace OVR;
using namespace OVR::Util::Render;

class CoinConverter
{
public:
	static SbRotation toSbRotation(QQuaternion &q);

	static SbVec3f toSbVec3f(const QVector3D &v);
	static SbVec3f toSbVec3f(const glm::vec3 &v);

	static QVector3D toQVector3D(const glm::vec3 &v);
	static SbMatrix toSbMatrix(const glm::mat4 &m);
};

#endif // CoinConverter_h__