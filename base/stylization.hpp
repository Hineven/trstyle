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