#-------------------------------------------------------------------------------
# LAGraph/experimental/test/CMakeLists.txt
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

#-------------------------------------------------------------------------------
# build the lagraphxtest library
#-------------------------------------------------------------------------------

include_directories ( ${CMAKE_SOURCE_DIR}/src/test/include
    ${CMAKE_SOURCE_DIR}/src/test/include
    ${CMAKE_SOURCE_DIR}/experimental/test/include )

file ( GLOB LAGRAPHXTEST_LIB_SOURCES "LG_*.c" )

# Uncomment this line for for development only, not for end-users:
# set ( CMAKE_BUILD_TYPE Debug )
if ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_DEBUG}" )
else ( )
    set ( CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_RELEASE}")
endif ( )

#-------------------------------------------------------------------------------
# dynamic lagraphxtest library properties
#-------------------------------------------------------------------------------

add_library ( lagraphxtest SHARED ${LAGRAPHXTEST_LIB_SOURCES} )
SET_TARGET_PROPERTIES ( lagraphxtest PROPERTIES
    VERSION ${LAGraph_VERSION_MAJOR}.${LAGraph_VERSION_MINOR}.${LAGraph_VERSION_SUB}
    SOVERSION ${LAGraph_VERSION_MAJOR}
    C_STANDARD_REQUIRED 11 )
set_property ( TARGET lagraphxtest PROPERTY C_STANDARD 11 )
target_link_libraries(lagraphxtest PUBLIC lagraph lagraphtest ${GRAPHBLAS_LIBRARIES})
target_link_directories( lagraphxtest BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )

#-------------------------------------------------------------------------------
# static lagraphxtest library properties
#-------------------------------------------------------------------------------

add_library ( lagraphxtest_static STATIC ${LAGRAPHXTEST_LIB_SOURCES} )
SET_TARGET_PROPERTIES ( lagraphxtest_static PROPERTIES
    VERSION ${LAGraph_VERSION_MAJOR}.${LAGraph_VERSION_MINOR}.${LAGraph_VERSION_SUB}
    OUTPUT_NAME lagraphxtest
    POSITION_INDEPENDENT_CODE OFF
    SOVERSION ${LAGraph_VERSION_MAJOR}
    C_STANDARD_REQUIRED 11 )

if ( MSVC )
    set_target_properties ( lagraphxtest_static PROPERTIES
        OUTPUT_NAME lagraphxtest_static )
endif ( )

set_property ( TARGET lagraphxtest_static PROPERTY C_STANDARD 11 )
target_link_libraries(lagraphxtest_static PUBLIC lagraph lagraphtest ${GRAPHBLAS_LIBRARIES})
target_link_directories( lagraphxtest_static BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )

#-------------------------------------------------------------------------------
# add OpenMP
#-------------------------------------------------------------------------------

if ( OPENMP_FOUND )
    target_link_libraries ( lagraphxtest PUBLIC ${OpenMP_C_LIBRARIES} )
    target_link_libraries ( lagraphxtest_static PUBLIC ${OpenMP_C_LIBRARIES} )
endif ( )


#-------------------------------------------------------------------------------
# This will only build tests from files with the name "test_*.c"
#-------------------------------------------------------------------------------

file( GLOB TEST_SOURCES LIST_DIRECTORIES false *_test.c test*.c )
foreach( testsourcefile ${TEST_SOURCES} )
    get_filename_component(justname ${testsourcefile} NAME)
    string( REPLACE ".c" "" testname ${justname} )
#   message("Adding: ${testname}")
    add_executable( ${testname} ${testsourcefile} )
    target_link_libraries( ${testname} lagraphx lagraph lagraphxtest lagraphtest ${GRAPHBLAS_LIBRARIES} )
    target_link_directories( ${testname} BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )
    string( REPLACE "test_" "ctest_" ctestname ${testname})
    add_test( ${ctestname} ${testname})
endforeach( testsourcefile ${TEST_SOURCES} )

