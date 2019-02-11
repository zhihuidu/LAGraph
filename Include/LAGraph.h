//------------------------------------------------------------------------------
// LAGraph.h:  include file for user applications that use LAGraph
//------------------------------------------------------------------------------

// LAGraph, (TODO list all authors here) (c) 2019, All Rights Reserved.
// http://graphblas.org  See LAGraph/Doc/License.txt for license.

//------------------------------------------------------------------------------

// TODO: add more comments to this file.

//------------------------------------------------------------------------------
// include files and global #defines
//------------------------------------------------------------------------------

#include "GraphBLAS.h"
#include <complex.h>

#define _POSIX_C_SOURCE 200809L
#include <time.h>

#if defined ( __linux__ )
#include <sys/time.h>
#endif

#if defined ( _OPENMP )
#include <omp.h>
#endif

#if defined ( __MACH__ )
#include <mach/clock.h>
#include <mach/mach.h>
#endif

#define LAGRAPH_RAND_MAX 32767

// suitable for integers, and non-NaN floating point:
#define LAGRAPH_MAX(x,y) (((x) > (y)) ? (x) : (y))
#define LAGRAPH_MIN(x,y) (((x) < (y)) ? (x) : (y))

// for floating-point, same as min(x,y,'omitnan') and max(...) in MATLAB
#define LAGRAPH_OMITNAN(x,y,f) ((isnan (x)) ? (y) : ((isnan (y)) ? (x) : (f)))
#define LAGRAPH_FMIN(x,y) LAGRAPH_OMITNAN (x, y, LAGRAPH_MIN (x,y))
#define LAGRAPH_FMAX(x,y) LAGRAPH_OMITNAN (x, y, LAGRAPH_MAX (x,y))

// free a block of memory and set the pointer to NULL
#define LAGRAPH_FREE(p)     \
{                           \
    LAGraph_free (p) ;      \
    p = NULL ;              \
}

//------------------------------------------------------------------------------
// LAGRAPH_OK: call LAGraph or GraphBLAS and check the result
//------------------------------------------------------------------------------

// To use LAGRAPH_OK, the #include'ing file must declare a scalar GrB_Info
// info, and must define LAGRAPH_FREE_ALL as a macro that frees all workspace
// if an error occurs.  The method can be a GrB_Info scalar as well, so that
// LAGRAPH_OK(info) works.  The function that uses this macro must return
// GrB_Info, or int.

#define LAGRAPH_ERROR(message,info)                                         \
{                                                                           \
    fprintf (stderr, "LAGraph error: %s\n[%d]\n%s\nFile: %s Line: %d\n",    \
        message, info, GrB_error ( ), __FILE__, __LINE__) ;                 \
    LAGRAPH_FREE_ALL ;                                                      \
    return (info) ;                                                         \
}

#define LAGRAPH_OK(method)                                                  \
{                                                                           \
    info = method ;                                                         \
    if (! (info == GrB_SUCCESS || info == GrB_NO_VALUE))                    \
    {                                                                       \
        LAGRAPH_ERROR ("", info) ;                                          \
    }                                                                       \
}

//------------------------------------------------------------------------------
// global objects
//------------------------------------------------------------------------------

// LAGraph_Complex is a GrB_Type containing the ANSI C11 double complex
// type.  This is required so that any arbitrary Matrix Market format
// can be read into GraphBLAS.
extern GrB_Type LAGraph_Complex ;

extern GrB_BinaryOp

    // binary operators to test for symmetry, skew-symmetry
    // and Hermitian property
    LAGraph_EQ_Complex          ,
    LAGraph_SKEW_INT8           ,
    LAGraph_SKEW_INT16          ,
    LAGraph_SKEW_INT32          ,
    LAGraph_SKEW_INT64          ,
    LAGraph_SKEW_FP32           ,
    LAGraph_SKEW_FP64           ,
    LAGraph_SKEW_Complex        ,
    LAGraph_Hermitian           ;

extern GrB_UnaryOp

    // unary operators to check if the entry is equal to 1
    LAGraph_ISONE_INT8          ,
    LAGraph_ISONE_INT16         ,
    LAGraph_ISONE_INT32         ,
    LAGraph_ISONE_INT64         ,
    LAGraph_ISONE_UINT8         ,
    LAGraph_ISONE_UINT16        ,
    LAGraph_ISONE_UINT32        ,
    LAGraph_ISONE_UINT64        ,
    LAGraph_ISONE_FP32          ,
    LAGraph_ISONE_FP64          ,
    LAGraph_ISONE_Complex       ,

    // unary operators that return 1
    LAGraph_TRUE_BOOL           ,
    LAGraph_TRUE_BOOL_Complex   ;

// monoids and semirings
extern GrB_Monoid

    LAGraph_PLUS_INT64_MONOID,
    LAGraph_MAX_INT32_MONOID,
    LAGraph_LAND_MONOID,
    LAGraph_LOR_MONOID ;

extern GrB_Semiring LAGraph_LOR_LAND_BOOL ;

// all 16 descriptors
// syntax: 4 characters define the following.  'o' is the default:
// 1: o or t: A transpose
// 2: o or t: B transpose
// 3: o or c: complemented mask
// 4: o or r: replace
extern GrB_Descriptor

    LAGraph_desc_oooo ,   // default (NULL)
    LAGraph_desc_ooor ,   // replace
    LAGraph_desc_ooco ,   // compl mask
    LAGraph_desc_oocr ,   // compl mask, replace

    LAGraph_desc_otoo ,   // A'
    LAGraph_desc_otor ,   // A', replace
    LAGraph_desc_otco ,   // A', compl mask
    LAGraph_desc_otcr ,   // A', compl mask, replace

    LAGraph_desc_tooo ,   // B'
    LAGraph_desc_toor ,   // B', replace
    LAGraph_desc_toco ,   // B', compl mask
    LAGraph_desc_tocr ,   // B', compl mask, replace

    LAGraph_desc_ttoo ,   // A', B'
    LAGraph_desc_ttor ,   // A', B', replace
    LAGraph_desc_ttco ,   // A', B', compl mask
    LAGraph_desc_ttcr ;   // A', B', compl mask, replace

//------------------------------------------------------------------------------
// user-callable utility functions
//------------------------------------------------------------------------------

GrB_Info LAGraph_init ( ) ;         // start LAGraph

GrB_Info LAGraph_finalize ( ) ;     // end LAGraph

GrB_Info LAGraph_mmread
(
    GrB_Matrix *A,      // handle of matrix to create
    FILE *f             // file to read from, already open
) ;

GrB_Info LAGraph_mmwrite
(
    GrB_Matrix A,           // matrix to write to the file
    FILE *f                 // file to write it to
    // TODO , FILE *fcomments         // optional file with extra comments
) ;

GrB_Info LAGraph_ispattern  // return GrB_SUCCESS if successful
(
    bool *result,           // true if A is all one, false otherwise
    GrB_Matrix A,
    GrB_UnaryOp userop      // for A with arbitrary user-defined type.
                            // Ignored if A and B are of built-in types or
                            // LAGraph_Complex.
) ;

GrB_Info LAGraph_pattern    // return GrB_SUCCESS if successful
(
    GrB_Matrix *C,          // a boolean matrix with the pattern of A
    GrB_Matrix A
) ;

GrB_Info LAGraph_isequal    // return GrB_SUCCESS if successful
(
    bool *result,           // true if A == B, false if A != B or error
    GrB_Matrix A,
    GrB_Matrix B,
    GrB_BinaryOp userop     // for A and B with arbitrary user-defined types.
                            // Ignored if A and B are of built-in types or
                            // LAGraph_Complex.
) ;

GrB_Info LAGraph_isall      // return GrB_SUCCESS if successful
(
    bool *result,           // true if A == B, false if A != B or error
    GrB_Matrix A,
    GrB_Matrix B,
    GrB_BinaryOp op         // GrB_EQ_<type>, for the type of A and B,
                            // to check for equality.  Or use any desired
                            // operator.  The operator should return GrB_BOOL.
) ;

uint64_t LAGraph_rand (uint64_t *seed) ;

uint64_t LAGraph_rand64 (uint64_t *seed) ;

double LAGraph_randx (uint64_t *seed) ;

GrB_Info LAGraph_random         // create a random matrix
(
    GrB_Matrix *A,              // handle of matrix to create
    GrB_Type type,              // built-in type, or LAGraph_Complex
    GrB_Index nrows,            // number of rows
    GrB_Index ncols,            // number of columns
    GrB_Index nvals,            // number of values
    bool make_pattern,          // if true, A is a pattern
    bool make_symmetric,        // if true, A is symmetric
    bool make_skew_symmetric,   // if true, A is skew-symmetric
    bool make_hermitian,        // if trur, A is hermitian
    bool no_diagonal,           // if true, A has no entries on the diagonal
    uint64_t *seed              // random number seed; modified on return
) ;

GrB_Info LAGraph_alloc_global ( ) ;

GrB_Info LAGraph_free_global ( ) ;

void *LAGraph_malloc        // wrapper for malloc
(
    size_t nitems,          // number of items
    size_t size_of_item     // size of each item
) ;

void LAGraph_free           // wrapper for free
(
    void *p
) ;

void LAGraph_tic            // gets current time in seconds and nanoseconds
(
    double tic [2]          // tic [0]: seconds, tic [1]: nanoseconds
) ;

double LAGraph_toc          // returns time since last LAGraph_tic
(
    const double tic [2]    // tic from last call to LAGraph_tic
) ;

//------------------------------------------------------------------------------
// user-callable algorithms
//------------------------------------------------------------------------------

GrB_Info LAGraph_bfs_pushpull   // push-pull BFS, or push-only if AT = NULL
(
    GrB_Vector *v_output,   // v(i) is the BFS level of node i in the graph
    const GrB_Matrix A,     // input graph, treated as if boolean in semiring
    const GrB_Matrix AT,    // transpose of A (optional; push-only if NULL)
    GrB_Index s,            // starting node of the BFS (s < 0: whole graph)
    int64_t max_level,      // see description above
    bool vsparse            // if true, v is expected to be very sparse
) ;

GrB_Info LAGraph_bfs_simple     // push-only BFS
(
    GrB_Vector *v_output,   // v(i) is the BFS level of node i in the graph
    const GrB_Matrix A,     // input graph, treated as if boolean in semiring
    GrB_Index s             // starting node of the BFS
) ;
