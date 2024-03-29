# Custom test target to run the googletest tests
add_custom_target(check)
add_custom_command(
        TARGET check
        POST_BUILD
        COMMENT "All tests were successful!" VERBATIM
)

# Custom test target to just build the googletest tests
add_custom_target(build_check)
add_custom_command(
        TARGET build_check
        POST_BUILD
        COMMENT "All test builds were successful!" VERBATIM
)

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/stamps)

# will compile and run ${test_target}.cpp
# and add all further arguments as dependencies
macro(generic_run_test test_target test_file
        driver driver_dep register_target register_build_target kind_name)
    set(options)
    set(oneValueArgs)
    set(multiValueArgs DEPS BIN_DEPS)
    cmake_parse_arguments(TEST_TARGET "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

    add_executable(${test_target}_testrunner
            EXCLUDE_FROM_ALL
            ${driver}
            ${test_file}
            )

    target_link_libraries(${test_target}_testrunner
            ${driver_dep}
            ${TEST_TARGET_DEPS}
            stdc++fs rt dl
            gsaca
            sdsl
            divsufsort
            )

    if(ENABLE_MALLOC_COUNT)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ansi")
        target_sources(${test_target}_testrunner PUBLIC ${PROJECT_SOURCE_DIR}/external/malloc_count/malloc_count)
        target_compile_definitions(${test_target}_testrunner PUBLIC MALLOC_COUNT)

        target_include_directories(${test_target}_testrunner PUBLIC
                $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/malloc_count/>
                $<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/external/malloc_count/>)
    endif(ENABLE_MALLOC_COUNT)

    target_include_directories(${test_target}_testrunner PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
            $<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/include/>
            )

    target_include_directories(${test_target}_testrunner PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/external/sdsl/include>
            $<INSTALL_INTERFACE:${PROJECT_SOURCE_DIR}/external/sdsl/include>
            )

    # Runs the test and generates a stamp file on success.
    add_custom_command(
            OUTPUT
            stamps/${test_target}_testrunner.stamp
            DEPENDS
            ${test_target}_testrunner
            COMMAND
            ${test_target}_testrunner
            COMMAND
            cmake -E touch ${CMAKE_CURRENT_BINARY_DIR}/stamps/${test_target}_testrunner.stamp
            WORKING_DIRECTORY
            "${CMAKE_BINARY_DIR}"
            COMMENT
            "Running ${kind_name} ${test_target} ..."
            VERBATIM
    )

    # The test target. Depends on the stamp file to ensure the
    # test is only run if the source changed
    add_custom_target(
            ${test_target}
            DEPENDS
            stamps/${test_target}_testrunner.stamp
    )

    # Hook into check target
    add_custom_command(
            TARGET ${register_target}
            PRE_BUILD
            COMMAND cmake --build . --target ${test_target}
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "${kind_name} ${test_target}" VERBATIM
    )

    # Hook into build_check target
    add_custom_command(
            TARGET ${register_build_target}
            PRE_BUILD
            COMMAND cmake --build . --target ${test_target}_testrunner
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
            COMMENT "Building ${kind_name} ${test_target}" VERBATIM
    )

    # Ensure binary deps of the testrunner are compiled first
    foreach(bin_dep ${TEST_TARGET_BIN_DEPS})
        add_custom_command(
                TARGET ${test_target}_testrunner
                PRE_BUILD
                COMMAND cmake --build . --target ${bin_dep}
                WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
        )
    endforeach(bin_dep)
endmacro()

macro(run_test test_target)
    generic_run_test(
            ${test_target}
            "${test_target}.cpp"
            "test_driver/test_driver.cpp"
            gtest
            check
            build_check
            "Test"
            ${ARGN}
    )
endmacro()

file(GLOB files "*.cpp")
foreach(file ${files})
    if(NOT ${file} MATCHES ".*/test_driver/test_driver.cpp")
        string(REGEX REPLACE ".*/" "" file "${file}")
        string(REPLACE ".cpp" "" file "${file}")
        run_test(${file})
    endif()
endforeach()