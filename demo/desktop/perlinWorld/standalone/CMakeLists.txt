if (WINDOWS)
    add_compile_definitions(WINDOWS)

    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
else ()
    add_link_options(-no-pie)
endif()

add_executable(perlinWorld "perlinWorld.cpp")

target_link_libraries(perlinWorld Hop)