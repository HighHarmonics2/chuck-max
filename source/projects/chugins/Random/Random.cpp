//-----------------------------------------------------------------------------
// Entaro ChucK Developer!
// This is a Chugin boilerplate, generated by chuginate!
//-----------------------------------------------------------------------------

// this should align with the correct versions of these ChucK files
#include "chuck_dl.h"
#include "chuck_def.h"

// general includes
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <math.h>


#ifdef _WIN32
static long random() { return rand(); }
static void srandom( unsigned s ) { srand( s ); }
#endif // _WIN32

#ifdef _WIN32
#define CK_RANDOM_MAX RAND_MAX
#else
#define CK_RANDOM_MAX 0x7fffffff
#endif


// example of getter/setter
CK_DLL_SFUN(Random_seed);
CK_DLL_SFUN(Random_gaussian);


CK_DLL_QUERY( Random )
{
    // hmm, don't change this...
    QUERY->setname(QUERY, "Random");
    
    QUERY->begin_class(QUERY, "Random", "Object");

    QUERY->add_sfun(QUERY, Random_seed, "void", "seed");
    QUERY->add_arg(QUERY, "int", "seed");

    QUERY->add_sfun(QUERY, Random_gaussian, "float", "gaussian");
    QUERY->add_arg(QUERY, "float", "mean");
    QUERY->add_arg(QUERY, "float", "stdv");

    // end the class definition
    // IMPORTANT: this MUST be called!
    QUERY->end_class(QUERY);

    // wasn't that a breeze?
    return TRUE;
}


CK_DLL_SFUN(Random_seed)
{
    t_CKINT seed = GET_NEXT_INT(ARGS);
    
    srandom(seed);
}


CK_DLL_SFUN(Random_gaussian)
{
    t_CKFLOAT mean = GET_NEXT_FLOAT(ARGS);
    t_CKFLOAT stdv = GET_NEXT_FLOAT(ARGS);
    
    // see http://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
    t_CKFLOAT U0 = random()/((float) CK_RANDOM_MAX);
    t_CKFLOAT U1 = random()/((float) CK_RANDOM_MAX);
    t_CKFLOAT Z0 = sqrt(-2*log(U0))*cos(2*CK_ONE_PI*U1);
    
    // set the return value
    RETURN->v_float = mean+Z0*stdv;
}
