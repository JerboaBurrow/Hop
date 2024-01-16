cmake_minimum_required(VERSION 3.22)
project(macos-lib-test)

set(OUTPUT_NAME user)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-trapping-math -fno-rounding-math -fno-signaling-nans -fno-signed-zeros")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -O3 -Wall")
        
set(SRC
    "main.cpp"
)

include_directories(include)
include_directories(.)

find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)
find_package(Vulkan REQUIRED)
find_package(OpenGL REQUIRED)
find_package(X11 REQUIRED)

add_executable(${OUTPUT_NAME} ${SRC})

target_link_libraries(
	${OUTPUT_NAME}
	${CMAKE_SOURCE_DIR}/libjGL.a
	${X11_LIBRARIES}
	${OPENGL_LIBRARIES} 
    ${Vulkan_LIBRARIES}
    	${ZLIB_LIBRARIES}
    	${PNG_LIBRARIES}
    ${CMAKE_SOURCE_DIR}/libHop.a
)
target_include_directories(
	${OUTPUT_NAME}
	PUBLIC
	${Vulkan_INCLUDE_DIR} 
)

# https://stackoverflow.com/questions/18391487/compiling-with-glfw3-linker-errors-undefined-reference
target_link_libraries(${OUTPUT_NAME} "-framework Cocoa -framework IOKit -framework CoreVideo")

target_compile_definitions(${OUTPUT_NAME} PUBLIC GLSL_VERSION="330")
