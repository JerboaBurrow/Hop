set(OUTPUT_NAME TestScriptPack)

include_directories(.)

if (WINDOWS)
    if (RELEASE)
        # launch as windows, not console app - so cmd does not open as well
        add_link_options(-mwindows)
    endif ()
else()
    # so nautilus etc recognise target as executable rather than .so
    add_link_options(-no-pie)
endif()

add_executable(${OUTPUT_NAME} "main.cpp")

target_link_libraries(${OUTPUT_NAME} Hop)

set_target_properties(${OUTPUT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}")

file(COPY "pack.scriptz" DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")

if (TEST_SUITE)
    add_test(NAME packedScripts COMMAND "/${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/${OUTPUT_NAME}")
    set_tests_properties(packedScripts PROPERTIES
        PASS_REGULAR_EXPRESSION "(this is a, requiring b)(.|\n)+(this is b)"
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/"
    )
endif()