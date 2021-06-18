#ifndef _H_PRETREATMENT
#define _H_PRETREATMENT

#include "heads.hpp"
#include "data.hpp"
#include "stylization.hpp"

namespace Init{
    extern int n_init;
    extern int num;
    extern Vector2 verts[100];
    struct Triangle {
        short v[3], t;
    };
    extern Triangle trigs[500];
    void InitializeMain(int _n_init);

}

#endif