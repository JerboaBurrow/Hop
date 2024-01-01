set(OUTPUT_NAME PerlinWorld-sfml)

include_directories(../../include/SFML-2.5.1/include)

if (WINDOWS)
    add_compile_definitions(WINDOWS)

    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
    set(SFML_USE_STATIC_STD_LIBS FALSE)
    set(SFML_STATIC_LIBRARIES FALSE)
    set(SFML_DIR "../../include/SFML-2.5.1-mingw64/lib/cmake/SFML")
else ()
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "../../include/SFML-2.5.1/build")
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

set(CMAKE_MODULE_PATH ../../include/SFML-2.5.1/cmake)

# graphics for sf::RenderWindow (window is in sfml-window)

find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)

add_executable(${OUTPUT_NAME}
    "sfmlPerlinWorld.cpp"
)

target_link_libraries(${OUTPUT_NAME}
    Hop
    sfml-graphics
)

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")

file(GLOB LUA "../*.lua")
file(GLOB MAP "../*.hmap")

file(COPY ${LUA} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")
file(COPY ${MAP} DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")