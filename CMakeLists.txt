project(OculusRiftExamples)

cmake_minimum_required(VERSION 2.8)

# Comment out these two lines to disable the debug build
set(CMAKE_BUILD_TYPE "Debug")

# Verbosity is good, especially if a command fails
set(CMAKE_VERBOSE_MAKEFILE ON)

# Add debug output from the Oculus SDK
add_definitions(-DOVR_BUILD_DEBUG)

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
endif()

###############################################################################
#
# Sample code support
#

# The primary SDK artifact, a static library for Oculus access 
add_subdirectory(libraries/OculusSDK/LibOVR)
# Make sure to set up the OVR include paths
include_directories(${CORE_OculusVR_SOURCE_DIR}/Include)
include_directories(${CORE_OculusVR_SOURCE_DIR}/Src)
include_directories(${CORE_OculusVR_SOURCE_DIR}/Src/Kernel)
include_directories(${CORE_OculusVR_SOURCE_DIR}/Src/Util)

set(EXAMPLE_LIBS OculusVR)

function(add_example SOURCE_FILES)
    if(WIN32)

        list(APPEND SOURCE_FILES ${EXAMPLE_RC_FILE} )
        add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES} )

    elseif(APPLE)

        list(APPEND SOURCE_FILES ${EXAMPLE_ALL_RESOURCES})
        set_source_files_properties(${EXAMPLE_ALL_RESOURCES} PROPERTIES MACOSX_PACKAGE_LOCATION Resources)
        add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE_FILES} )

    else()

        list(APPEND SOURCE_FILES ${EXAMPLE_ALL_RESOURCES})
        file(COPY ${EXAMPLE_ALL_RESOURCES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Resources)
        add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

    endif()

    source_group("Resources/Shaders" FILES ${EXAMPLE_SHADERS})
    source_group("Resources/Meshes" FILES ${EXAMPLE_MESHES})
    target_link_libraries(${PROJECT_NAME}  ${EXAMPLE_LIBS})

endfunction()

add_subdirectory(source/CrashDemo)
