cmake_minimum_required(VERSION 2.8)

project(CoinOculus)

#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

SET(CMAKE_PREFIX_PATH C:/Qt/5.1.1/msvc2010_opengl)	
#SET(CMAKE_PREFIX_PATH C:/Qt/5.2.1/5.2.1/msvc2010_opengl)	
find_package(Qt5 REQUIRED Core Concurrent Declarative Gui Network OpenGL Script Sql Test Xml XmlPatterns Widgets)
#find_package(OpenGL)

# find_package(Coin3D)
if(WIN32)
set(INVENTOR_INCLUDES ${CMAKE_SOURCE_DIR}/libraries/Coin_3.1.3_vs10/include)
set(COIN3D_LIBRARIES optimized ${CMAKE_SOURCE_DIR}/libraries/Coin_3.1.3_vs10/lib/coin3.lib
                     debug     ${CMAKE_SOURCE_DIR}/libraries/Coin_3.1.3_vs10/lib/coin3d.lib)
else(WIN32)
set(COIN3D_LIBRARIES Coin)
endif(WIN32)

SET(INVENTOR_DEFS -DCOIN_DLL -DQUARTER_NOT_DLL)


INCLUDE_DIRECTORIES(
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
	source/Example00/CubeValues.h
	)
		
ADD_DEFINITIONS(
	${INVENTOR_DEFS}
)

LINK_DIRECTORIES(
	${CMAKE_SOURCE_DIR}/libraries/OculusLib/Win32
)
	
ADD_EXECUTABLE(OculusTestGL source/Example00/Example00.cpp ${IPOCTLG_SRC}  ${IPOCTLG_MOC} ${IPOCTLG_UI})
TARGET_LINK_LIBRARIES(OculusTestGL  
	Qt5::Declarative
	Qt5::Script
	Qt5::Xml
	Qt5::OpenGL
	Qt5::Widgets
	Qt5::Gui
	Qt5::Core
	
	${COIN3D_LIBRARIES}
	
	opengl32 
	glu32
	debug libovrd.lib
	optimized libovr.lib
	winmm.lib
	glew
)

ADD_EXECUTABLE(OculusTestBugExample MinimalBuggingExample/main.cpp MinimalBuggingExample/main.h)
TARGET_LINK_LIBRARIES(OculusTestBugExample  
	Qt5::Declarative
	Qt5::Script
	Qt5::Xml
	Qt5::OpenGL
	Qt5::Widgets
	Qt5::Gui
	Qt5::Core
	
	${COIN3D_LIBRARIES}
	
	opengl32 
	glu32
	debug libovrd.lib
	optimized libovr.lib
	winmm.lib
	glew
)

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