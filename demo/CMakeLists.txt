
include_directories(include/SFML-2.5.1/include)
if (WINDOWS)
    add_compile_definitions(WINDOWS)
if (RELEASE)
    # launch as windows, not console app - so cmd does not open as well
    add_link_options(-mwindows)
endif ()
    set(SFML_USE_STATIC_STD_LIBS FALSE)
    set(SFML_STATIC_LIBRARIES FALSE)
    set(SFML_DIR "include/SFML-2.5.1-mingw64/lib/cmake/SFML")
else ()
    set(SFML_STATIC_LIBRARIES TRUE)
    set(SFML_DIR "include/SFML-2.5.1/build")
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

# graphics for sf::RenderWindow (window is in sfml-window)
find_package(SFML 2.5.1 COMPONENTS graphics REQUIRED)

add_executable(sfmlPerlinWorld
    "sfmlPerlinWorld.cpp"
)

target_link_libraries(sfmlPerlinWorld
    Hop
    sfml-graphics
)