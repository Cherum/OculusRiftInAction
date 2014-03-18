#include <GL/glew.h>

#include <QApplication>
#include <QKeyEvent>

#include <QGLShaderProgram>
#include <QGLShader>
#include <QGLWidget>
//#include <qthread.h>

#include "GLprogram.h"
#include "OculusRenderer.h"

//
//#ifdef __APPLE__
//	#include "CoreFoundation/CFBundle.h"
//#endif

//#include "OVR.h"
//#undef new

int main(int argc, char ** argv) {

//#ifdef WIN32
//    int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
//#else
//    int main(int argc, char ** argv) {
//
//    // Window and Apple both support applications embedded resources.  For
//    // Linux we're going to try to locate the shaders relative to the
//    // executable
//    #ifndef __APPLE__
//        string executable(argv[0]);
//        string::size_type sep = executable.rfind('/');
//        if (sep != string::npos) {
//            executableDirectory = executable.substr(0, sep) + "/Resources";
//        }
//    #endif
//#endif

	QApplication app(argc, argv);

	OculusRenderer renderer = OculusRenderer();

	// widget
	renderer.show();
	app.exec();

	 // Clean up resources.
	//root->unref();
	//delete viewer;
	/*SIM::Coin3D::Quarter::Quarter::clean();
	return 0;*/

	//return exmpl.run();
}