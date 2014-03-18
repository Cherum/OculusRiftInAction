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

#include "CoinConverter.h"

using namespace OVR;
using namespace OVR::Util::Render;

SbRotation CoinConverter::toSbRotation(QQuaternion &q){
	return SbRotation(q.x(), q.y(), q.z(),q.scalar());
}

SbVec3f CoinConverter::toSbVec3f(const QVector3D &v ){
	return SbVec3f(v.x(), v.y(), v.z()); 
}
SbVec3f CoinConverter::toSbVec3f(const glm::vec3 &v ){
	return SbVec3f(v.x, v.y, v.z); 
}
QVector3D CoinConverter::toQVector3D(const glm::vec3 &v ){
	return QVector3D(v.x, v.y, v.z);
}

SbMatrix CoinConverter::toSbMatrix(const glm::mat4 &m){
	return SbMatrix(
		m[0][0], m[0][1], m[0][2], m[0][3],
		m[1][0], m[1][1], m[1][2], m[1][3],
		m[2][0], m[2][1], m[2][2], m[2][3],
		m[3][0], m[3][1], m[3][2], m[3][3]
	);
}