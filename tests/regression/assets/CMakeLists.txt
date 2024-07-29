set(OUTPUT_NAME TestAssetStore)

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

file(COPY "resource" DESTINATION "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/")

if (TEST_SUITE)
    if(WINDOWS)
        add_test(NAME assetStore COMMAND "${CMAKE_CROSSCOMPILING_EMULATOR}" "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/${OUTPUT_NAME}.exe")
    else()
        add_test(NAME assetStore COMMAND "/${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/${OUTPUT_NAME}")
    endif()
    set_tests_properties(assetStore PROPERTIES
        PASS_REGULAR_EXPRESSION "\"texture/HEART.png\" \"texture/Pi.png\" \"texture/random.png\""
        WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/${OUTPUT_NAME}/"
    )
endif()