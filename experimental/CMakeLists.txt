#-------------------------------------------------------------------------------
# LAGraph/experimental/CMakeLists.txt:  cmake script for LAGraph/experimental
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

include_directories ( ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src/utility
    ${CMAKE_SOURCE_DIR}/deps/json_h )

file ( GLOB LAGRAPHX_LIB_SOURCES "utility/*.c" "algorithm/*.c" )

#-------------------------------------------------------------------------------
# dynamic lagraphx library properties
#-------------------------------------------------------------------------------

add_library ( lagraphx SHARED ${LAGRAPHX_LIB_SOURCES} )
SET_TARGET_PROPERTIES ( lagraphx PROPERTIES
    VERSION ${LAGraph_VERSION_MAJOR}.${LAGraph_VERSION_MINOR}.${LAGraph_VERSION_SUB}
    SOVERSION ${LAGraph_VERSION_MAJOR}
    C_STANDARD_REQUIRED 11
    PUBLIC_HEADER "include/LAGraphX.h" )
set_property ( TARGET lagraphx PROPERTY C_STANDARD 11 )
target_link_libraries ( lagraphx PUBLIC lagraph ${GRAPHBLAS_LIBRARIES} )
target_link_directories ( lagraphx BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )

#-------------------------------------------------------------------------------
# static lagraphx library properties
#-------------------------------------------------------------------------------

add_library ( lagraphx_static STATIC ${LAGRAPHX_LIB_SOURCES} )
SET_TARGET_PROPERTIES ( lagraphx_static PROPERTIES
    VERSION ${LAGraph_VERSION_MAJOR}.${LAGraph_VERSION_MINOR}.${LAGraph_VERSION_SUB}
    OUTPUT_NAME lagraphx
    POSITION_INDEPENDENT_CODE OFF
    SOVERSION ${LAGraph_VERSION_MAJOR}
    C_STANDARD_REQUIRED 11 )

if ( MSVC )
    set_target_properties ( lagraphx_static PROPERTIES
        OUTPUT_NAME lagraphx_static )
endif ( )

set_property ( TARGET lagraphx_static PROPERTY C_STANDARD 11 )
target_link_libraries ( lagraphx_static PUBLIC lagraph_static ${GRAPHBLAS_LIBRARIES} )
target_link_directories ( lagraphx_static BEFORE PUBLIC ${CMAKE_SOURCE_DIR}/build )

#-------------------------------------------------------------------------------
# add OpenMP
#-------------------------------------------------------------------------------

if ( OPENMP_FOUND )
    target_link_libraries ( lagraphx PUBLIC lagraph ${OpenMP_C_LIBRARIES} )
    target_link_libraries ( lagraphx_static PUBLIC lagraph_static ${OpenMP_C_LIBRARIES} )
endif ( )

#-------------------------------------------------------------------------------
# applications
#-------------------------------------------------------------------------------

add_subdirectory ( test )
add_subdirectory ( benchmark )

