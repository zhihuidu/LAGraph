//------------------------------------------------------------------------------
// LAGraph/src/test/test_Property_AT.c:  test LAGraph_Property_AT
//------------------------------------------------------------------------------

// LAGraph, (c) 2021 by The LAGraph Contributors, All Rights Reserved.
// SPDX-License-Identifier: BSD-2-Clause
//
// See additional acknowledgments in the LICENSE file,
// or contact permission@sei.cmu.edu for the full terms.

//------------------------------------------------------------------------------

#include "LAGraph_test.h"

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------

LAGraph_Graph G = NULL ;
char msg [LAGRAPH_MSG_LEN] ;
GrB_Matrix A = NULL, B = NULL ;
GrB_Type atype = NULL ;
#define LEN 512
char filename [LEN+1] ;

//------------------------------------------------------------------------------
// setup: start a test
//------------------------------------------------------------------------------

void setup (void)
{
    OK (LAGraph_Init (msg)) ;
}

//------------------------------------------------------------------------------
// teardown: finalize a test
//------------------------------------------------------------------------------

void teardown (void)
{
    OK (LAGraph_Finalize (msg)) ;
}

//------------------------------------------------------------------------------
// test_Property_AT:  test LAGraph_Property_AT
//------------------------------------------------------------------------------

typedef struct
{
    LAGraph_Kind kind ;
    const char *name ;
}
matrix_info ;

const matrix_info files [ ] =
{
    LAGRAPH_ADJACENCY_DIRECTED,   "cover.mtx",
    LAGRAPH_ADJACENCY_DIRECTED,   "ldbc-directed-example.mtx",
    LAGRAPH_ADJACENCY_UNDIRECTED, "ldbc-undirected-example.mtx",
    LAGRAPH_UNKNOWN,              ""
} ;

void test_Property_AT (void)
{
    setup ( ) ;

    for (int k = 0 ; ; k++)
    {

        // load the matrix as A
        const char *aname = files [k].name ;
        int kind = files [k].kind ;
        if (strlen (aname) == 0) break;
        TEST_CASE (aname) ;
        snprintf (filename, LEN, LG_DATA_DIR "%s", aname) ;
        FILE *f = fopen (filename, "r") ;
        TEST_CHECK (f != NULL) ;
        OK (LAGraph_MMRead (&A, &atype, f, msg)) ;
        OK (fclose (f)) ;
        // GxB_print (A, 2) ;
        TEST_MSG ("Loading of adjacency matrix failed") ;

        // construct the graph G with adjacency matrix A
        OK (LAGraph_New (&G, &A, atype, kind, msg)) ;
        TEST_CHECK (A == NULL) ;

        // create the G->AT property
        OK (LAGraph_Property_AT (G, msg)) ;

        // try to create it again; this should safely do nothing
        OK (LAGraph_Property_AT (G, msg)) ;

        // check the result
        if (kind == LAGRAPH_ADJACENCY_UNDIRECTED)
        {
            TEST_CHECK (G->AT == NULL) ;
        }
        else
        {
            // ensure G->A and G->AT are transposed of each other;
            // B = (G->AT)'
            // GxB_print (G->AT, 2) ;
            GrB_Index nrows, ncols ;
            OK (GrB_Matrix_nrows (&nrows, G->A)) ;
            OK (GrB_Matrix_nrows (&ncols, G->A)) ;
            OK (GrB_Matrix_new (&B, atype, nrows, ncols)) ;
            OK (GrB_transpose (B, NULL, NULL, G->AT, NULL)) ;

            // ensure B and G->A are the same
            bool GA_and_B_are_identical ;
            OK (LAGraph_IsEqual (&GA_and_B_are_identical, G->A, B, NULL, msg)) ;
            TEST_CHECK (GA_and_B_are_identical) ;
            TEST_MSG ("Test for G->A and B equal failed") ;
            // GxB_print (B, 2) ;
            OK (GrB_free (&B)) ;
        }

        OK (LAGraph_Delete (&G, msg)) ;
    }

    teardown ( ) ;
}

//-----------------------------------------------------------------------------
// TEST_LIST: the list of tasks for this entire test
//-----------------------------------------------------------------------------

TEST_LIST =
{
    { "Property_AT", test_Property_AT },
    { NULL, NULL }
} ;
