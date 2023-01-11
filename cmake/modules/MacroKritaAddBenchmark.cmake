# SPDX-FileCopyrightText: 2010 Cyrille Berger <cberger@cberger.net>
# SPDX-FileCopyrightText: 2006-2009 Alexander Neundorf <neundorf@kde.org>
# SPDX-FileCopyrightText: 2006, 2007 Laurent Montel <montel@kde.org>
# SPDX-FileCopyrightText: 2007 Matthias Kretz <kretz@kde.org>
#
# SPDX-License-Identifier: BSD-3-Clause
#

add_custom_target(benchmark)

include(KritaTestSuite)

macro (KRITA_ADD_BENCHMARK _test_NAME)

    set(_srcList ${ARGN})
    set(_targetName ${_test_NAME})
    if( ${ARGV1} STREQUAL "TESTNAME" )
        set(_targetName ${ARGV2})
        list(REMOVE_AT _srcList 0 1)
    endif()

    set(_nogui)
    list(GET ${_srcList} 0 first_PARAM)
    if( ${first_PARAM} STREQUAL "NOGUI" )
        set(_nogui "NOGUI")
    endif()

    add_executable( ${_test_NAME} ${_srcList} )
    set_test_sdk_compile_definitions(${_test_NAME})
    ecm_mark_as_test(${_test_NAME})

    if(NOT KDE4_TEST_OUTPUT)
        set(KDE4_TEST_OUTPUT plaintext)
    endif()
    set(KDE4_TEST_OUTPUT ${KDE4_TEST_OUTPUT} CACHE STRING "The output to generate when running the QTest unit tests")

    set(using_qtest "")
    foreach(_filename ${_srcList})
        if(NOT using_qtest)
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${_filename}")
                file(READ ${_filename} file_CONTENT)
                string(REGEX MATCH "QTEST_(KDE)?MAIN" using_qtest "${file_CONTENT}")
            endif()
        endif()
    endforeach()

    if (using_qtest AND KDE4_TEST_OUTPUT STREQUAL "xml")
        #MESSAGE(STATUS "${_targetName} : Using QTestLib, can produce XML report.")
        add_custom_target( ${_targetName} COMMAND $<TARGET_FILE:${_test_NAME}> -xml -o ${_targetName}.tml )
    else ()
        #MESSAGE(STATUS "${_targetName} : NOT using QTestLib, can't produce XML report, please use QTestLib to write your unit tests.")
        add_custom_target( ${_targetName} COMMAND $<TARGET_FILE:${_test_NAME}> )
    endif ()

    add_dependencies(benchmark ${_targetName} )
#    add_test( ${_targetName} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_test_NAME} -xml -o ${_test_NAME}.tml )

    if (NOT MSVC_IDE)   #not needed for the ide
        # if the tests are EXCLUDE_FROM_ALL, add a target "buildtests" to build all tests
        if (NOT BUILD_TESTING)
           if(NOT TARGET buildtests)
              add_custom_target(buildtests)
           endif()
           add_dependencies(buildtests ${_test_NAME})
        endif ()
    endif ()
endmacro (KRITA_ADD_BENCHMARK)
