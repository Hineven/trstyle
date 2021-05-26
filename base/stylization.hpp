/*
 * Filename: /home/hineven/prj/trstyle/rouine.hpp
 * Path: /home/hineven/prj/trstyle
 * Created Date: Monday, May 17th 2021, 5:27:31 pm
 * Author: Hineven
 * 
 * Copyright (c) 2021 QwQ Unlimited Company
 */
#ifndef _H_ROUTINE
#define _H_ROUTINE
#include "heads.hpp"
#include "data.hpp"
#include "calculation.hpp"


namespace Stylization {

class Vector2Grad: public Vector2 {
public:
    Vector2 grad {0};
    inline Vector2Grad () = default;
    inline explicit Vector2Grad (const Float & f): Vector2(f) {}
    inline Vector2Grad (Float a, Float b): Vector2(a, b) {assert(!isNan(x) && !isNan(y));}
    Vector2Grad & operator = (const Vector2 & t) {
        x = t.x;
        y = t.y;
        return *this;
    }
};
// Trianglulation vertices
extern std::vector<Vector2Grad> vertices;
struct Triangle {
    // Indices of triangle vertices, sorted in anti-clockwise order spatially
    short v[3], t;
    inline Vector2 operator [] (int i) const {return vertices[v[i]];}
    inline Vector2 & operator [] (int i) {return vertices[v[i]];}
};

// Initialize the stylization process with two big triangles
// Note: the function Data::loadImage must be called before
// this function
void initializeSimple () ;

// Initialize the stylization process with some existing trianglulation
// Note: the function Data::loadImage must be called before
// running this function
void initializeWith (
    const Vector2 * verts, int nverts,
    const Triangle * triangles, int ntriangles
) ;

// Initialize first, and iterate.
// Just iterate, there's nothing the caller needs to worry about.
void iterate () ;

// Clear everything. It's useless if the program only runs once.
// Also, its functionality has not been tested. So if a bug came
// out because  of this function --
// Dont blame me.
void clear () ;

}

#endif