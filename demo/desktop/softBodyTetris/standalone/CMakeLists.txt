set(OUTPUT_NAME SoftBodyTetris)

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

add_executable(${OUTPUT_NAME} "main.cpp")

target_link_libraries(${OUTPUT_NAME} Hop)

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")

file(GLOB LUA "../*.lua")
file(GLOB MAP "../*.hmap")

file(COPY ${LUA} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")
file(COPY ${MAP} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")