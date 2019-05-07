cmake_minimum_required(VERSION 3.4.1)

SET( GCC_COVERAGE_LINK_FLAGS    "-v")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -Wno-unused-function")
SET( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( CMAKE_SHARED_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( CMAKE_STATIC_LINKER_FLAGS  "${CMAKE_STATIC_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
SET( OSG_CPP_EXCEPTIONS_AVAILABLE "Yes")

SET(KERNEL_LIBRARIES
    GLESv3
	EGL
	jnigraphics
	log
	m)
SET(GLM_DIRECTORY "D:/OpenGL-Libs/glm")
include_directories(${GLM_DIRECTORY})
add_library(laprasNative
            SHARED
            src/main/cpp/jni_interface.cpp
            src/main/cpp/dcmRenderer.cpp
			src/main/cpp/AndroidHelper.cpp
			src/main/cpp/perfMonitor.cpp
            )

target_link_libraries(laprasNative
                      ${CVR_PLUGINS}
                      android
                      mediandk
                      ${KERNEL_LIBRARIES})