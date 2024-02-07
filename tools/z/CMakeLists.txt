set(OUTPUT_NAME z)

include_directories(.)

if (WINDOWS)
    add_compile_definitions(WINDOWS)

    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
else ()
    add_link_options(-no-pie)
endif()

add_executable(${OUTPUT_NAME} "main.cpp" "${PROJECT_SOURCE_DIR}/src/Util/z.cpp")

target_link_libraries(${OUTPUT_NAME} zlibstatic)

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")
