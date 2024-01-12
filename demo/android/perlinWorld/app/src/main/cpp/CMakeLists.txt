
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.22.1)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE MinSizeRel)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -std=c++17")
# Declares and names the project.
project("HopAndroid")

add_library(
        HopAndroid
        SHARED
        hopAndroid.cpp)

target_compile_definitions(HopAndroid PUBLIC GLSL_VERSION="300 es")

include_directories(include)
include_directories(.)
include_directories(include/vendored/glm)
include_directories(include/jGL/include)
include_directories(include/vendored/freetype/include)
include_directories(include/vendored/stduuid/include)
include_directories(include/vendored/stduuid/)
include_directories(include/vendored/lua/src)
include_directories(include/vendored)
include_directories(include/vendored/vorbis/include)
include_directories(include/vendored/ogg/include)


link_directories(include)

find_library(
              log-lib
              log )

find_library(GLES-lib
        GLESv3)

find_library(android-lib
        android
        )

target_compile_definitions(HopAndroid PUBLIC BENCHMARK)
target_compile_definitions(HopAndroid PUBLIC ANDROID)
set(libHop ${CMAKE_SOURCE_DIR}/include/libHop-${CMAKE_ANDROID_ARCH_ABI}.a)
set(libjGL ${CMAKE_SOURCE_DIR}/include/libjGL-${CMAKE_ANDROID_ARCH_ABI}.a)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       HopAndroid
                       ${libjGL}
                       ${libHop}
                       # Links the target library to the log library
                       # included in the NDK.
                       ${log-lib}
                       ${GLES-lib}
                       ${android-lib}
        )
