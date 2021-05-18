/*
 * Filename: /home/hineven/prj/trstyle/calculation.hpp
 * Path: /home/hineven/prj/trstyle
 * Created Date: Monday, May 17th 2021, 7:21:23 pm
 * 
 * Copyright (c) 2021 QwQ Unlimited Company
 */
#ifndef _H_CALCULATION
#define _H_CALCULATION

#include "heads.hpp"
#include "data.hpp"


// Render a single triangle
// Currently using constant coloring, regardless of the style set in data.hpp
// This procedure should be executed on GPU for further acceleration
void render (Vector2 a, Vector2 b, Vector2 c, Float * buffer = nullptr) ;


// Render the triangle, later calculate the energy of a certain triangle
// ifrender tells if the render function should render the triangle to
// the output buffer. Otherwise it will use the buffer for calculation.
// This procedure should be executed on GPU for further acceleration
Float energy (Vector2 a, Vector2 b, Vector2 c, bool ifrender, Float * buffer = nullptr) ;

#endif