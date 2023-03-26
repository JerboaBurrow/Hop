cmake_minimum_required(VERSION 3.1)
project(Jerboa)

option(SYNTAX_ONLY "only check syntax" OFF)
option(TEST_SUITE "test" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(RELEASE On)
option(SANITISE OFF)
option(BUILD_DEMOS OFF)

set(CMAKE_CXX_STANDARD 17)
if (RELEASE)
    set(CMAKE_BUILD_TYPE Release)
else()
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (SYNTAX_ONLY)
#     add_compile_options(-fsyntax-only)
#     # graphics for sf::RenderWindow (window is in sfml-window)
#     find_package(OpenGL REQUIRED)
#     set(GLEW_INCLUDE include/vendored/glew/include/)
#     include_directories(
#         include 
#         ${OPENGL_INCLUDE_DIRS} 
#         ${GLEW_INCLUDE} 
#         include/vendored/freetype/include 
#         include/vendored/glm
#     )
#     file(GLOB SRC "src/*.cpp")
#     file(GLOB TEXT_SRC "src/Text/*.cpp")
#     add_executable(Jerboa ${SRC} ${TEXT_SRC})

else()

string(TIMESTAMP TODAY "%Y-%m-%d:%H:%M:%S")
add_compile_definitions(TIMESTAMP="${TODAY}")

if (RELEASE)
    add_compile_definitions(BUILD_TYPE="Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3")
else ()
    add_compile_definitions(BUILD_TYPE="Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g3")
endif ()

if (SANITISE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
# if .so's are included nautilus wont be able to find them
# once moved to a different folder with the .so's and .out
# without setting this, seemed to work in terminal though!
set(CMAKE_BUILD_RPATH ".")

add_subdirectory(include/vendored/zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib_build)
include_directories(include/vendored/zlib ${CMAKE_CURRENT_BINARY_DIR}/zlib_build)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
SET(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE)
SET(CMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE)
# force freetype to use the static zlib build
SET(FT_WITH_HARFBUZZ OFF)
SET(FT_WITH_BZIP2 OFF)
SET(CMAKE_DISABLE_FIND_PACKAGE_HarfBuzz=TRUE)
SET(CMAKE_DISABLE_FIND_PACKAGE_BZip2=TRUE)
add_subdirectory(include/vendored/freetype)
include_directories(include/vendored/freetype/include)
SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so")
add_subdirectory(include/vendored/glm)
add_subdirectory(include/vendored/stduuid)

if (ANDROID)
    find_library(GLES-lib GLESv3 REQUIRED)
else()
    find_package(OpenGL REQUIRED)
    # let sources know glew is static
    add_compile_definitions(GLEW_STATIC)
    set(GLEW_SOURCE include/vendored/glew/src/glew.c)
    set(GLEW_INCLUDE include/vendored/glew/include/)

    add_library(glew STATIC ${GLEW_SOURCE} ${GLEW_INCLUDE})

    target_link_libraries(glew
        ${OPENGL_LIBRARY}
    )

endif()

include_directories(include ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE} ${ZLIB_DEPS_DIR})

file(GLOB SRC "src/*.cpp")
file(GLOB TEXT_SRC "src/Text/*.cpp")
file(GLOB OBJECT_SRC "src/Object/*.cpp")
file(GLOB SHADER_SRC "src/Shader/*.cpp")
file(GLOB SYSTEM_SRC "src/System/*.cpp")
file(GLOB COMPONENT_SRC "src/Component/*.cpp") 
file(GLOB COLLISION_SRC "src/Collision/*.cpp")
file(GLOB WORLD_SRC "src/World/*.cpp") 
file(GLOB MATHS_SRC "src/Maths/*.cpp")
file(GLOB UTIL_SRC "src/Util/*.cpp")
add_library(Hop STATIC
    ${SHADER_SRC} 
    ${SRC} 
    ${TEXT_SRC} 
    ${OBJECT_SRC} 
    ${SYSTEM_SRC} 
    ${COMPONENT_SRC} 
    ${COLLISION_SRC}
    ${WORLD_SRC}
    ${MATHS_SRC}
    ${UTIL_SRC}
)

if (ANDROID)
    target_compile_definitions(Hop PUBLIC GLSL_VERSION="300 es")
    target_link_libraries(Hop ${GLES-lib} zlibstatic FreeType glm stduuid)
else()
    target_compile_definitions(Hop PUBLIC GLSL_VERSION="140")
    target_link_libraries(Hop glew zlibstatic FreeType glm stduuid ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})
endif()


IF (BUILD_DEMOS)

    IF (ANDROID)
    else()
        add_subdirectory(demo/desktop)
    endif()

ENDIF()

IF (TEST_SUITE)

    IF (ANDROID)
    else()
        IF (WINDOWS)
            ## ask nicely for big objs (gnu assembler flag)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-mbig-obj")
        endif()

        file(GLOB TEST_SRC "tests/*.cpp")
        include_directories(include "tests/")

        add_executable(testSuite 
            ${TEST_SRC}
            "src/World/mapFile.cpp"
        )

        target_link_libraries(testSuite stduuid zlibstatic freetype glm)
        
        include(CTest)
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
        include(Catch)
        catch_discover_tests(testSuite)
    endif()
  
ENDIF(TEST_SUITE)

endif()
