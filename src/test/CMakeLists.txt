#-------------------------------------------------------------------------------
# LAGraph/src/test/CMakeLists.txt:  cmake script for LAGraph/src/test
#-------------------------------------------------------------------------------

# LAGraph, (c) 2019-2022 by The LAGraph Contributors, All Rights Reserved.
# SPDX-License-Identifier: BSD-2-Clause
#
# For additional details (including references to third party source code and
# other files) see the LICENSE file or contact permission@sei.cmu.edu. See
# Contributors.txt for a full list of contributors. Created, in part, with
# funding and support from the U.S. Government (see Acknowledgments.txt file).
# DM22-0790

#-------------------------------------------------------------------------------

# Most tests can be done with any GraphBLAS library, but extensive ("brutal")
# memory test can only be done with SuiteSparse:GraphBLAS.

#-------------------------------------------------------------------------------
# build the lagraphtest library
#-------------------------------------------------------------------------------

include_directories ( ${CMAKE_SOURCE_DIR}/src/test/include
    ${CMAKE_SOURCE_DIR}/src/algorithm )

file ( GLOB LAGRAPHTEST_LIB_SOURCES "LG_*.c" )

# Uncomment this line for for development only, not for end-users:
# set ( CMAKE_BUILD_TYPE Debug )
if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" )
else ( )
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
endif ( )

#-------------------------------------------------------------------------------
# dynamic lagraphtest library properties
#-------------------------------------------------------------------------------

add_library ( lagraphtest SHARED ${LAGRAPHTEST_LIB_SOURCES} )
SET_TARGET_PROPERTIES ( lagraphtest PROPERTIES
    VERSION ${LAGraph_VERSION_MAJOR}.${LAGraph_VERSION_MINOR}.${LAGraph_VERSION_SUB}
    SOVERSION ${LAGraph_VERSION_MAJOR}
    C_STANDARD_REQUIRED 11 )
set_property ( TARGET lagraphtest PROPERTY C_STANDARD 11 )
target_link_libraries( lagraphtest PUBLIC lagraph ${GRAPHBLAS_LIBRARIES})
target_link_directories( lagraphtest BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )

#-------------------------------------------------------------------------------
# static lagraphtest library properties
#-------------------------------------------------------------------------------

add_library ( lagraphtest_static STATIC ${LAGRAPHTEST_LIB_SOURCES} )
SET_TARGET_PROPERTIES ( lagraphtest_static PROPERTIES
    VERSION ${LAGraph_VERSION_MAJOR}.${LAGraph_VERSION_MINOR}.${LAGraph_VERSION_SUB}
    OUTPUT_NAME lagraphtest
    POSITION_INDEPENDENT_CODE OFF
    SOVERSION ${LAGraph_VERSION_MAJOR}
    C_STANDARD_REQUIRED 11 )

if ( MSVC )
    set_target_properties ( lagraphtest_static PROPERTIES
        OUTPUT_NAME lagraphtest_static )
endif ( )

set_property ( TARGET lagraphtest_static PROPERTY C_STANDARD 11 )
target_link_libraries(lagraphtest_static PUBLIC lagraph ${GRAPHBLAS_LIBRARIES})
target_link_directories( lagraphtest_static BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )

#-------------------------------------------------------------------------------
# add OpenMP
#-------------------------------------------------------------------------------

if ( OPENMP_FOUND )
    target_link_libraries ( lagraphtest PUBLIC ${OpenMP_C_LIBRARIES} )
    target_link_libraries ( lagraphtest_static PUBLIC ${OpenMP_C_LIBRARIES} )
endif ( )

#-------------------------------------------------------------------------------
# This will only build tests from files with the name "test_*.c"
#-------------------------------------------------------------------------------

file( GLOB TEST_SOURCES LIST_DIRECTORIES false test_*.c )
foreach( testsourcefile ${TEST_SOURCES} )
    get_filename_component(justname ${testsourcefile} NAME)
    string( REPLACE ".c" "" testname ${justname} )
#   message("Adding: ${testname}")
    add_executable( ${testname} ${testsourcefile})
    target_link_libraries( ${testname} lagraph lagraphx lagraphtest ${GRAPHBLAS_LIBRARIES} )
    target_link_directories( ${testname} BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )
    string( REPLACE "test_" "ctest_" ctestname ${testname})
    add_test( ${ctestname} ${testname})
endforeach( testsourcefile ${TEST_SOURCES} )

