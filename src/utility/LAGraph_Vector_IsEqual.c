//------------------------------------------------------------------------------
// LAGraph_Vector_IsEqual: check two vectors for exact equality
//------------------------------------------------------------------------------

// LAGraph, (c) 2021 by The LAGraph Contributors, All Rights Reserved.
// SPDX-License-Identifier: BSD-2-Clause
//
// See additional acknowledgments in the LICENSE file,
// or contact permission@sei.cmu.edu for the full terms.

//------------------------------------------------------------------------------

// LAGraph_Vector_IsEqual, contributed by Tim Davis, Texas A&M

// Checks if two vectors are identically equal (same size,
// type, pattern, size, and values).

// See also LAGraph_IsEqual.

// For both methods, if the two vectors are GrB_FP32, GrB_FP64, or related,
// and have NaNs, then these functions will return false,
// since NaN == NaN is false.  To check for NaN equality (like
// isequalwithequalnans in MATLAB), use LAGraph_isall with a user-defined
// operator f(x,y) that returns true if x and y are both NaN.

#define LAGraph_FREE_WORK GrB_free (&C) ;

#include "LG_internal.h"

//------------------------------------------------------------------------------
// LAGraph_Vector_IsEqual_op:  compare two vectors using a given operator
//------------------------------------------------------------------------------

GrB_Info LAGraph_Vector_IsEqual_op    // return GrB_SUCCESS if successful
(
    bool *result,           // true if A == B, false if A != B or error
    GrB_Vector A,
    GrB_Vector B,
    GrB_BinaryOp userop,    // comparator to use (required)
    char *msg
)
{
    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    LG_CLEAR_MSG ;
    GrB_Vector C = NULL ;
    LG_ASSERT (userop != NULL && result != NULL, GrB_NULL_POINTER) ;

    GrB_Info info ;

    //--------------------------------------------------------------------------
    // check for NULL and aliased vectors
    //--------------------------------------------------------------------------

    if (A == NULL || B == NULL || A == B)
    {
        // two NULL vectors are identical, as are two aliased matrices
        (*result) = (A == B) ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // compare the size of A and B
    //--------------------------------------------------------------------------

    GrB_Index nrows1, nrows2;
    GrB_TRY (GrB_Vector_size (&nrows1, A)) ;
    GrB_TRY (GrB_Vector_size (&nrows2, B)) ;
    if (nrows1 != nrows2)
    {
        // # of rows differ
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // compare the # entries in A and B
    //--------------------------------------------------------------------------

    GrB_Index nvals1, nvals2 ;
    GrB_TRY (GrB_Vector_nvals (&nvals1, A)) ;
    GrB_TRY (GrB_Vector_nvals (&nvals2, B)) ;
    if (nvals1 != nvals2)
    {
        // # of entries differ
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // C = A .* B, where the pattern of C is the intersection of A and B
    //--------------------------------------------------------------------------

    GrB_TRY (GrB_Vector_new (&C, GrB_BOOL, nrows1)) ;
    GrB_TRY (GrB_eWiseMult (C, NULL, NULL, userop, A, B, NULL)) ;

    //--------------------------------------------------------------------------
    // ensure C has the same number of entries as A and B
    //--------------------------------------------------------------------------

    GrB_Index nvals ;
    GrB_TRY (GrB_Vector_nvals (&nvals, C)) ;
    if (nvals != nvals1)
    {
        // pattern of A and B are different
        LAGraph_FREE_WORK ;
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // result = and (C)
    //--------------------------------------------------------------------------

    GrB_TRY (GrB_reduce (result, NULL, GrB_LAND_MONOID_BOOL, C, NULL)) ;

    //--------------------------------------------------------------------------
    // free workspace and return result
    //--------------------------------------------------------------------------

    // return result
    LAGraph_FREE_WORK ;
    return (GrB_SUCCESS) ;
}


//------------------------------------------------------------------------------
// LAGraph_Vector_IsEqual:  compare two vectors
//------------------------------------------------------------------------------

int LAGraph_Vector_IsEqual         // returns 0 if successful, < 0 if failure
(
    bool *result,           // true if A == B, false if A != B or error
    GrB_Vector A,
    GrB_Vector B,
    char *msg
)
{

    //--------------------------------------------------------------------------
    // check inputs
    //--------------------------------------------------------------------------

    LG_CLEAR_MSG ;
    GrB_Vector C = NULL ;
    LG_ASSERT (result != NULL, GrB_NULL_POINTER) ;

    GrB_Info info ;

    //--------------------------------------------------------------------------
    // check for NULL and aliased vectors
    //--------------------------------------------------------------------------

    if (A == NULL || B == NULL || A == B)
    {
        // two NULL vectors are identical, as are two aliased matrices
        (*result) = (A == B) ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // compare the type of A and B
    //--------------------------------------------------------------------------

    char atype_name [LAGRAPH_MAX_NAME_LEN] ;
    char btype_name [LAGRAPH_MAX_NAME_LEN] ;
    LG_TRY (LAGraph_VectorTypeName (atype_name, A, msg)) ;
    LG_TRY (LAGraph_VectorTypeName (btype_name, B, msg)) ;
    if (!MATCHNAME (atype_name, btype_name))
    {
        // types differ
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // compare the size of A and B
    //--------------------------------------------------------------------------

    GrB_Index nrows1, nrows2;
    GrB_TRY (GrB_Vector_size (&nrows1, A)) ;
    GrB_TRY (GrB_Vector_size (&nrows2, B)) ;
    if (nrows1 != nrows2)
    {
        // # of rows differ
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // compare the # entries in A and B
    //--------------------------------------------------------------------------

    GrB_Index nvals1, nvals2 ;
    GrB_TRY (GrB_Vector_nvals (&nvals1, A)) ;
    GrB_TRY (GrB_Vector_nvals (&nvals2, B)) ;
    if (nvals1 != nvals2)
    {
        // # of entries differ
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // get the GrB_EQ_type operator
    //--------------------------------------------------------------------------

    GrB_Type type ;
    LG_TRY (LAGraph_TypeFromName (&type, atype_name, msg)) ;
    GrB_BinaryOp op ;
    // select the comparator operator
    if      (type == GrB_BOOL  ) op = GrB_EQ_BOOL   ;
    else if (type == GrB_INT8  ) op = GrB_EQ_INT8   ;
    else if (type == GrB_INT16 ) op = GrB_EQ_INT16  ;
    else if (type == GrB_INT32 ) op = GrB_EQ_INT32  ;
    else if (type == GrB_INT64 ) op = GrB_EQ_INT64  ;
    else if (type == GrB_UINT8 ) op = GrB_EQ_UINT8  ;
    else if (type == GrB_UINT16) op = GrB_EQ_UINT16 ;
    else if (type == GrB_UINT32) op = GrB_EQ_UINT32 ;
    else if (type == GrB_UINT64) op = GrB_EQ_UINT64 ;
    else if (type == GrB_FP32  ) op = GrB_EQ_FP32   ;
    else if (type == GrB_FP64  ) op = GrB_EQ_FP64   ;
    #if 0
    else if (type == GxB_FC32  ) op = GxB_EQ_FC32   ;
    else if (type == GxB_FC64  ) op = GxB_EQ_FC64   ;
    #endif
    else
    {
        LG_ASSERT_MSG (false, GrB_NOT_IMPLEMENTED, "unsupported type") ;    // FIXME:RETVAL
    }

    //--------------------------------------------------------------------------
    // C = A .* B, where the pattern of C is the intersection of A and B
    //--------------------------------------------------------------------------

    GrB_TRY (GrB_Vector_new (&C, GrB_BOOL, nrows1)) ;
    GrB_TRY (GrB_eWiseMult (C, NULL, NULL, op, A, B, NULL)) ;

    //--------------------------------------------------------------------------
    // ensure C has the same number of entries as A and B
    //--------------------------------------------------------------------------

    GrB_Index nvals ;
    GrB_TRY (GrB_Vector_nvals (&nvals, C)) ;
    if (nvals != nvals1)
    {
        // pattern of A and B are different
        LAGraph_FREE_WORK ;
        (*result) = false ;
        return (GrB_SUCCESS) ;
    }

    //--------------------------------------------------------------------------
    // result = and (C)
    //--------------------------------------------------------------------------

    GrB_TRY (GrB_reduce (result, NULL, GrB_LAND_MONOID_BOOL, C, NULL)) ;

    //--------------------------------------------------------------------------
    // free workspace and return result
    //--------------------------------------------------------------------------

    // return result
    LAGraph_FREE_WORK ;
    return (GrB_SUCCESS) ;
}

