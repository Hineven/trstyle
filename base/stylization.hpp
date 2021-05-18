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

class Triangle;

// Initialize the stylization process with two big triangles
void initializeSimple () ;

// Initialize the stylization process with some existing trianglulation
void initializeWith (
    const Vector2 * verts, int nverts,
    const Triangle * triangles, int ntriangles
) ;

// Iterate. 
//
//
//
//
//
//
// What else are you expecting really?
void iterate () ;

// Clear everything
void clear () ;

}

#endif