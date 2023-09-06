cmake_minimum_required(VERSION 3.1)
project(Jerboa)

list(APPEND CMAKE_CXX_SOURCE_FILE_EXTENSIONS shader)

option(SYNTAX_ONLY "only check syntax" OFF)
option(TEST_SUITE "test" OFF)
option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(RELEASE On)
option(SANITISE OFF)
option(BUILD_DEMOS OFF)
option(BENCHMARK OFF)
option(STANDALONE OFF)

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
    if (ANDROID)
        message("ANDROID MinSizeRel!")
        # GOOGLE!! (Release is not a thing pos ndk 23 apparently..)
        add_compile_definitions(BUILD_TYPE="MinSizeRel")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signed-zeros")
    else()
        add_compile_definitions(BUILD_TYPE="Release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")

else ()
    if (ANDROID)
        # GOOGLE!!
        add_compile_definitions(BUILD_TYPE="RelWithDebInfo")
    else()
        add_compile_definitions(BUILD_TYPE="Debug")
    endif()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -g -Wall")
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

if (!WINDOWS)
    add_compile_definitions(LUA_USE_POSIX)
endif()

file(GLOB LUA_SRC "include/vendored/lua/src/*.c")
include_directories(include/vendored/lua/src)
add_library(Lua STATIC ${LUA_SRC})

if (ANDROID)
    find_library(GLES-lib GLESv3 REQUIRED)
else()

    add_subdirectory(include/vendored/glfw)
    include_directories(include/vendored/glfw/include)

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

include_directories(include include/vendored ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE} ${ZLIB_DEPS_DIR})

include_directories(src)

file(GLOB SRC 
    "src/*.cpp"
    "src/Text/*.cpp"
    "src/Object/*.cpp"
    "src/Shader/*.cpp"
    "src/System/*.cpp"
    "src/Component/*.cpp"
    "src/Collision/*.cpp"
    "src/World/*.cpp" 
    "src/Maths/*.cpp" 
    "src/Util/*.cpp"
    "src/Debug/*.cpp"
    "src/Console/*.cpp"
    "src/System/Rendering/*.cpp"
)

if (NOT ANDROID)
    file(GLOB STANDALONE_SRC "src/Display/*.cpp")
    add_library(Hop STATIC ${SRC} ${STANDALONE_SRC})
else()
    add_library(Hop STATIC ${SRC})
endif()

if(BENCHMARK)
    target_compile_definitions(Hop PUBLIC BENCHMARK)
endif()

if (ANDROID)
    target_compile_definitions(Hop PUBLIC GLSL_VERSION="300 es")
    target_link_libraries(Hop ${GLES-lib} zlibstatic FreeType glm stduuid Lua)
else()
    target_compile_definitions(Hop PUBLIC GLSL_VERSION="330")
    target_link_libraries(Hop glew zlibstatic FreeType glm stduuid ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} glfw Lua)
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

        file(GLOB TEST_SRC "tests/*.cpp" "src/Maths/*.cpp")
        include_directories(include "tests/")

        add_executable(testSuite 
            ${TEST_SRC}
            "src/World/mapFile.cpp"
        )

        target_compile_definitions(testSuite PUBLIC GLSL_VERSION="330")

        target_link_libraries(testSuite stduuid zlibstatic freetype glm)
        
        include(CTest)
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/tests/cmake/)
        include(Catch)
        catch_discover_tests(testSuite)
    endif()
  
ENDIF(TEST_SUITE)

endif()
