project(CoinOculus)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(Qt5_COMPONENTS Core Concurrent Declarative Gui Network OpenGL Script Sql Test Xml XmlPatterns Widgets)
find_package(Qt5 REQUIRED ${Qt5_COMPONENTS})
find_package(Qt5AxContainer QUIET)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0)
set(QCA_LIB_SUFFIX -qt5)

# find_package(Coin3D)
if(WIN32)
set(INVENTOR_INCLUDES ${CMAKE_SOURCE_DIR}/libraries/Coin_3.1.3_vs10/include)
set(COIN3D_LIBRARIES optimized ${CMAKE_SOURCE_DIR}/libraries/Coin_3.1.3_vs10/lib/coin3.lib
                     debug     ${CMAKE_SOURCE_DIR}/libraries/Coin_3.1.3_vs10/lib/coin3d.lib)
else(WIN32)
set(COIN3D_LIBRARIES Coin)
endif(WIN32)

SET(INVENTOR_DEFS -DCOIN_DLL -DQUARTER_NOT_DLL)

# Set to own Qt Path
SET(QT_INCLUDE_DIR C:/Qt/5.1.1/msvc2010_opengl)	

INCLUDE_DIRECTORIES(
	${QT_INCLUDE_DIR}/include
	${QT_INCLUDE_DIR}/include/QtCore
	${QT_INCLUDE_DIR}/include/QtGui
	${QT_INCLUDE_DIR}/include/QtXml
	${QT_INCLUDE_DIR}/include/QtOpenGL
	${QT_INCLUDE_DIR}/include/QtNetwork
	${QT_INCLUDE_DIR}/include/QtDeclarative
	${QT_INCLUDE_DIR}/include/QtWidgets
	${QT_INCLUDE_DIR}/include/QtScript
	${QT_INCLUDE_DIR}/mkspecs/win32-msvc2010
	
	${INVENTOR_INCLUDES}

	${CMAKE_CURRENT_BINARY_DIR}

	${CMAKE_SOURCE_DIR}/libraries/OculusSDK/LibOVR/Include
	${CMAKE_SOURCE_DIR}/libraries/glew/include
	${CMAKE_SOURCE_DIR}/libraries/glm
	)

SET(IPOCTLG_SRC 
	source/Example00/Distort.fs
	source/Example00/Distort.vs
	source/Example00/Example00.cpp
	source/Example00/Example00.rc
	source/Example00/resource.h
	source/Example00/Simple.fs
	source/Example00/Simple.vs
	source/Example00/Texture.fs
	source/Example00/Texture.vs
	source/Example00/GLprogram.cpp
	source/Example00/GLprogram.h
	source/Example00/OculusRenderer.cpp
	source/Example00/OculusRenderer.h
	source/Example00/CoinConverter.cpp
	source/Example00/CoinConverter.h
	source/Example00/DebugHelper.h
	)
		
ADD_DEFINITIONS(
	${INVENTOR_DEFS}
	)

LINK_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/libraries/OculusLib/Win32
	# ${CMAKE_SOURCE_DIR}/libraries/glew/include
	# ${CMAKE_SOURCE_DIR}/libraries/glm
	)
	
ADD_EXECUTABLE(OculusTestGL source/Example00/Example00.cpp ${IPOCTLG_SRC}  ${IPOCTLG_MOC} ${IPOCTLG_UI})
TARGET_LINK_LIBRARIES(OculusTestGL  
	${QT_LIBRARIES}
	
	${QT_INCLUDE_DIR}/lib/Qt5Declaratived.lib
	${QT_INCLUDE_DIR}/lib/Qt5Scriptd.lib
	${QT_INCLUDE_DIR}/lib/Qt5Xmld.lib
	${QT_INCLUDE_DIR}/lib/Qt5OpenGLd.lib
	${QT_INCLUDE_DIR}/lib/Qt5Widgetsd.lib
	${QT_INCLUDE_DIR}/lib/Qt5Guid.lib
	${QT_INCLUDE_DIR}/lib/Qt5Cored.lib  
	${COIN3D_LIBRARIES}
	${QT_QTOPENGL_LIBRARY}
	opengl32 
	glu32
	debug libovrd.lib
	optimized libovr.lib
	winmm.lib
	glew
)

# Comment out these two lines to disable the debug build
set(CMAKE_BUILD_TYPE "Debug")

# Verbosity is good, especially if a command fails
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add debug output from the Oculus SDK
add_definitions(-DOVR_BUILD_DEBUG)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=gnu++0x)
    if("${CMAKE_SYSTEM_PROCESSOR}" STREQUAL "x86_64")
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -fPIC"  )
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC"  )    
    endif()
endif()

if(APPLE)
	set(CMAKE_XCODE_ATTRIBUTE_GCC_VERSION "com.apple.compilers.llvm.clang.1_0")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
	set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++ -g -Wall")
endif()

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

###############################################################################
#
# Sample code support
#

# The Linux version of the community SDK requires an addition EDID handling library
if(UNIX AND NOT(APPLE) AND NOT(CYGWIN))
    add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/3rdParty/EDID)
endif()

# Cross platform access to shader functionality
add_definitions( -DGLEW_STATIC )
#add_subdirectory(libraries/glew)
add_subdirectory(${CMAKE_SOURCE_DIR}/libraries/glew)
#include_directories(libraries/glew/include)

# OpenGL vector / matrix header only math library
#include_directories(libraries/glm)