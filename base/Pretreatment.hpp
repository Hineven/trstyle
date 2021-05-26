#ifndef _H_PRETREATMENT
#define _H_PRETREATMENT

#include "heads.hpp"
#include "data.hpp"
#include "stylization.hpp"

namespace Init{
    int n;
    int num;
    Vector2 verts[100];
    struct Triangle {
        short v[3], t;
    };
    Triangle trigs[500];
    void InitializeMain();

}

#endif