cmake_minimum_required(VERSION 3.4.1)

add_library(ARCalVR_GPU
            SHARED
            src/main/cpp/jni_interface.cpp
            src/main/cpp/allController.cpp
            src/main/cpp/perfMonitor.cpp)

#arcore
add_library(arcore SHARED IMPORTED)
set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
                      ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so)

target_link_libraries( ARCalVR_GPU
                       android
                       mediandk
                       arcore
                       )