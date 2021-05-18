/*
 * Filename: /home/hineven/prj/trstyle/calculation.cpp
 * Path: /home/hineven/prj/trstyle
 * Created Date: Monday, May 17th 2021, 7:20:32 pm
 * 
 * Copyright (c) 2021 QwQ Unlimited Company
 */

#include "calculation.hpp"
#include "data.hpp"

// Render a single triangle
// Currently using constant coloring, regardless of the style set in data.hpp
// This procedure should be executed on GPU for further acceleration
void render (Vector2 a, Vector2 b, Vector2 c, Float * buffer) {
    if(buffer == nullptr) buffer = Data::image_output;
    if(cross(b-a, c-a) < 0) std::swap(b, c);
    int ymn, ymx;
    ymn = std::max(0, (int)floor(std::min(a.y, std::min(b.y, c.y))));
    ymx = std::min(Data::image_height-1, (int)ceil(std::max(a.y, std::max(b.y, c.y))));
    Color sum(0);
    int npixels = 0;
    // Loop over the triangle area
    for(int y = ymn; y<=ymx; y++) {
        int xmn = 0, xmx = Data::image_width-1;
        // Carefully calculate boundaries
        if(!equal(a.y, b.y)) {
            Vector2 p = a+(b-a)*((y+0.5-a.y)/(b.y-a.y));
            if(b.y < a.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(a.y > y+0.5 && a.x < b.x) xmx = -1;
            if(a.y < y+0.5 && a.x > b.x) xmx = -1;
        }
        if(!equal(b.y, c.y)) {
            Vector2 p = b+(c-b)*((y+0.5-b.y)/(c.y-b.y));
            if(c.y < b.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(b.y > y+0.5 && b.x < c.x) xmx = -1;
            if(b.y < y+0.5 && b.x > c.x) xmx = -1;
        }
        if(!equal(c.y, a.y)) {
            Vector2 p = c+(a-c)*((y+0.5-c.y)/(a.y-c.y));
            if(a.y < c.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(c.y > y+0.5 && c.x < a.x) xmx = -1;
            if(c.y < y+0.5 && c.x > a.x) xmx = -1;
        }
        // Boundaries found, loop over each valid pixel
        for(int x = xmn; x<=xmx; x++) {
            assert(x >= 0 && x < Data::image_width);
            assert(y >= 0 && y < Data::image_height);
            sum += Data::pixelColor(Data::image, x, y);
            npixels ++;
        }
    }
    // The average color of the triangle
    if(npixels != 0) sum = sum/npixels;
    for(int y = ymn; y<=ymx; y++) {
        int xmn = 0, xmx = Data::image_width-1;
        // Carefully calculate boundaries
        if(!equal(a.y, b.y)) {
            Vector2 p = a+(b-a)*((y+0.5-a.y)/(b.y-a.y));
            if(b.y < a.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(a.y > y+0.5 && a.x < b.x) xmx = -1;
            if(a.y < y+0.5 && a.x > b.x) xmx = -1;
        }
        if(!equal(b.y, c.y)) {
            Vector2 p = b+(c-b)*((y+0.5-b.y)/(c.y-b.y));
            if(c.y < b.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(b.y > y+0.5 && b.x < c.x) xmx = -1;
            if(b.y < y+0.5 && b.x > c.x) xmx = -1;
        }
        if(!equal(c.y, a.y)) {
            Vector2 p = c+(a-c)*((y+0.5-c.y)/(a.y-c.y));
            if(a.y < c.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(c.y > y+0.5 && c.x < a.x) xmx = -1;
            if(c.y < y+0.5 && c.x > a.x) xmx = -1;
        }
        // Constant coloring
        for(int x = xmn; x<=xmx; x++) 
            Data::pixelSetColor(buffer, x, y, sum);
    }
}


// Render the triangle, later calculate the energy of a certain triangle
// ifrender tells if the render function should render the triangle to
// the output buffer. Otherwise it will use the buffer for calculation.
// This procedure should be executed on GPU for further acceleration
Float energy (Vector2 a, Vector2 b, Vector2 c, bool ifrender, Float * buffer) {
    if(!ifrender && !buffer) fatal("Proper parameters needed.");
    if(buffer == nullptr) buffer = Data::image_output;
    if(cross(b-a, c-a) < 0) std::swap(b, c);
    render(a, b, c, buffer);
    int ymn, ymx;
    ymn = std::max(0, (int)floor(std::min(a.y, std::min(b.y, c.y))));
    ymx = std::min(Data::image_height-1, (int)ceil(std::max(a.y, std::max(b.y, c.y))));
    Float ret = 0, cnt = 0;
    // Loop over the triangle area
    for(int y = ymn; y<=ymx; y++) {
        int xmn = 0, xmx = Data::image_width-1;
        // Carefully calculate boundaries
        if(!equal(a.y, b.y)) {
            Vector2 p = a+(b-a)*((y+0.5-a.y)/(b.y-a.y));
            if(b.y < a.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(a.y > y+0.5 && a.x < b.x) xmx = -1;
            if(a.y < y+0.5 && a.x > b.x) xmx = -1;
        }
        if(!equal(b.y, c.y)) {
            Vector2 p = b+(c-b)*((y+0.5-b.y)/(c.y-b.y));
            if(c.y < b.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(b.y > y+0.5 && b.x < c.x) xmx = -1;
            if(b.y < y+0.5 && b.x > c.x) xmx = -1;
        }
        if(!equal(c.y, a.y)) {
            Vector2 p = c+(a-c)*((y+0.5-c.y)/(a.y-c.y));
            if(a.y < c.y) xmn = std::max(xmn, (int)ceil(p.x-0.5));
            else xmx = std::min(xmx, (int)floor(p.x-0.5));
        } else {
            if(c.y > y+0.5 && c.x < a.x) xmx = -1;
            if(c.y < y+0.5 && c.x > a.x) xmx = -1;
        }
        // Boundaries found, loop over each valid pixel
        for(int x = xmn; x<=xmx; x++) {
            Color s = Data::pixelColor(Data::image, x, y);
            Color t = Data::pixelColor(buffer, x, y);
            ret += (s.r-t.r)*(s.r-t.r)
                 + (s.g-t.g)*(s.g-t.g)
                 + (s.b-t.b)*(s.b-t.b);
            cnt ++;
            if(ifrender && buffer != Data::image_output) 
                Data::pixelSetColor(Data::image_output, x, y, t);
        }
    }
    //printf("energy:(%.2f,%.2f) (%.2f, %.2f) (%.2f, %.2f) %f, density:%f\n", a.x, a.y, b.x, b.y, c.x, c.y, ret, ret/cnt);
    //getchar();
    return ret;
}