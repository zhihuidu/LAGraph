//------------------------------------------------------------------------------
// LAGraph_NameOfType: return the C name of a type
//------------------------------------------------------------------------------

// LAGraph, (c) 2021 by The LAGraph Contributors, All Rights Reserved.
// SPDX-License-Identifier: BSD-2-Clause
// Contributed by Tim Davis, Texas A&M University.

//------------------------------------------------------------------------------

#include "LG_internal.h"

int LAGraph_NameOfType      // returns 0 if successful, < 0 if failure
(
    char *name,             // name of the type
    GrB_Type type,          // GraphBLAS type
    char *msg
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    LG_CLEAR_MSG ;
    LG_ASSERT (type != NULL, GrB_NULL_POINTER) ;
    LG_ASSERT (name != NULL, GrB_NULL_POINTER) ;

    //--------------------------------------------------------------------------
    // determine the name of the type
    //--------------------------------------------------------------------------

    if      (type == GrB_BOOL  ) strcpy (name, "bool")     ;
    else if (type == GrB_INT8  ) strcpy (name, "int8_t")   ;
    else if (type == GrB_INT16 ) strcpy (name, "int16_t")  ;
    else if (type == GrB_INT32 ) strcpy (name, "int32_t")  ;
    else if (type == GrB_INT64 ) strcpy (name, "int64_t")  ;
    else if (type == GrB_UINT8 ) strcpy (name, "uint8_t")  ;
    else if (type == GrB_UINT16) strcpy (name, "uint16_t") ;
    else if (type == GrB_UINT32) strcpy (name, "uint32_t") ;
    else if (type == GrB_UINT64) strcpy (name, "uint64_t") ;
    else if (type == GrB_FP32  ) strcpy (name, "float")    ;
    else if (type == GrB_FP64  ) strcpy (name, "double")   ;
    #if 0
    else if (type == GxB_FC32  ) strcpy (name, "float complex") ;
    else if (type == GxB_FC64  ) strcpy (name, "double complex") ;
    #endif
    else
    {
        name [0] = '\0' ;
        LG_ASSERT_MSG (false, GrB_NOT_IMPLEMENTED,
            "user-defined types not supported") ;
    }

    return (GrB_SUCCESS) ;
}

