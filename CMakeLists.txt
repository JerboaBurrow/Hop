cmake_minimum_required(VERSION 3.1)
project(Jerboa)

option(SYNTAX_ONLY "only check syntax" OFF)
option(TEST_SUITE "test" OFF)

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)
option(RELEASE On)
set(CMAKE_CXX_STANDARD 17)
if (RELEASE)
    set(CMAKE_BUILD_TYPE Release)
else()
    set(CMAKE_BUILD_TYPE Debug)
endif()

if (SYNTAX_ONLY)
    add_compile_options(-fsyntax-only)
    # graphics for sf::RenderWindow (window is in sfml-window)
    find_package(OpenGL REQUIRED)
    set(GLEW_INCLUDE include/vendored/glew/include/)
    include_directories(
        include 
        ${OPENGL_INCLUDE_DIRS} 
        ${GLEW_INCLUDE} 
        include/vendored/freetype/include 
        include/vendored/glm
        include/SFML-2.5.1/include
    )
    file(GLOB SRC "src/*.cpp")
    file(GLOB TEXT_SRC "src/Text/*.cpp")
    add_executable(Jerboa ${SRC} ${TEXT_SRC})

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

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
# if .so's are included nautilus wont be able to find them
# once moved to a different folder with the .so's and .out
# without setting this, seemed to work in terminal though!
set(CMAKE_BUILD_RPATH ".")

add_subdirectory(include/vendored/freetype)
add_subdirectory(include/vendored/glm)
add_subdirectory(include/vendored/stduuid)

find_package(OpenGL REQUIRED)
# let sources know glew is static
add_compile_definitions(GLEW_STATIC)
set(GLEW_SOURCE include/vendored/glew/src/glew.c)
set(GLEW_INCLUDE include/vendored/glew/include/)

include_directories(include ${OPENGL_INCLUDE_DIRS} ${GLEW_INCLUDE})

add_library(glew STATIC ${GLEW_SOURCE} ${GLEW_INCLUDE})

target_link_libraries(glew
	${OPENGL_LIBRARY}
)

if (WINDOWS)
    add_compile_definitions(WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
    set(SFML_USE_STATIC_STD_LIBS FALSE)
    set(SFML_STATIC_LIBRARIES FALSE)
    set(SFML_DIR "include/SFML-2.5.1-mingw64/lib/cmake/SFML/")

else ()
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "include/SFML-2.5.1/build")
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

# graphics for sf::RenderWindow (window is in sfml-window)
find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)
file(GLOB SRC "src/*.cpp")
file(GLOB TEXT_SRC "src/Text/*.cpp")
file(GLOB OBJECT_SRC "src/Object/*.cpp")
file(GLOB SHADER_SRC "src/Shader/*.cpp")
file(GLOB SYSTEM_SRC "src/System/*.cpp")
file(GLOB COMPONENT_SRC "src/Component/*.cpp") 
file(GLOB COLLISION_SRC "src/Collision/*.cpp")
file(GLOB WORLD_SRC "src/World/*.cpp") 
add_executable(Jerboa 
    ${SHADER_SRC} 
    ${SRC} 
    ${TEXT_SRC} 
    ${OBJECT_SRC} 
    ${SYSTEM_SRC} 
    ${COMPONENT_SRC} 
    ${COLLISION_SRC}
    ${WORLD_SRC}
)

target_link_libraries(Jerboa glew freetype glm stduuid sfml-graphics ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES})

IF (TEST_SUITE)

  file(GLOB TEST_SRC "tests/*.cpp")
  include_directories(include "tests/")

  add_executable(testSuite ${TEST_SRC})

  target_link_libraries(testSuite stduuid freetype glm)

ENDIF(TEST_SUITE)

endif()
