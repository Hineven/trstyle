/*
 * Filename: /home/hineven/prj/trstyle/heads.hpp
 * Path: /home/hineven/prj/trstyle
 * Created Date: Sunday, May 16th 2021, 10:49:08 pm
 * Author: Hineven
 * 
 * Copyright (c) 2021 Your Company
 */
#ifndef _H_HEADS
#define _H_HEADS
#include <thread>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <vector>
#include <ctime>
#include <assert.h>
#include <queue>
#include <functional>

#define Infinity 1e10

// Log a fatal message and quit
inline void fatal (std::string s) {
    std::cerr << "Fatal: " << s << std::endl;
    exit(-1); 
}
typedef float Float;

constexpr Float epsilon = 1e-3;

inline bool equal (const Float & a, const Float & b) {
    return std::abs(a-b) < epsilon;
}

class Vector2;


inline bool isNan (const Float & f) {return f != f;}
inline bool isNan (const Vector2 & v) ;

class Vector2 {
	public:
    Float x, y;
    inline Vector2 (): x(0), y(0) {}
    inline explicit Vector2 (const Float & f): x(f),  y(f) {}
    inline Vector2 (Float a, Float b): x(a), y(b) {assert(!isNan(x) && !isNan(y));}
    inline Vector2 operator + (const Vector2 & b) const {return Vector2(x+b.x, y+b.y);}
    inline Vector2 operator - (const Vector2 & b) const {return Vector2(x-b.x, y-b.y);}
    inline Vector2 operator * (const Float & b) const {assert(!isNan(b)); return Vector2(x*b, y*b);}
    inline Vector2 operator / (const Float & b) const {assert(b != 0); return Vector2(x/b, y/b);}
    inline Vector2 & operator += (const Vector2 & b) {x += b.x, y += b.y; return *this;}
    inline Vector2 & operator -= (const Vector2 & b) {x -= b.x, y -= b.y; return *this;}
    inline Vector2 & operator *= (const Float & b) {assert(!isNan(b)); x *= b, y *= b; return * this;}
    inline Vector2 & operator /= (const Float & b) {assert(b != 0); x /= b, y /= b; return * this;}
    inline Vector2 operator - () const {return Vector2(-x, -y);}
    inline Float lengthSquared () const {return x*x+y*y;}
    inline Vector2 normalized () const {
        Float fac = 1/sqrt(lengthSquared());
        assert(!isNan(fac));
        return (*this)*fac;
    }
    inline void resize (const Float & f) {
        Float fac = (f/sqrt(lengthSquared()));
        assert(!isNan(fac));
        *this = *this*fac;
    }
    inline Vector2 resized (const Float & f) const {
        Float fac = (f/sqrt(lengthSquared()));
        assert(!isNan(fac));
        return (*this)*fac;
    }
};

inline Vector2 operator * (const Float & a, const Vector2 & b) {
    return {a*b.x, a*b.y};
}

inline std::ostream & operator << (std::ostream & os, const Vector2 & vec) {
    os << '(' << vec.x << ',' << vec.y << ')';
    return os;
}

inline Float dot (const Vector2 & a, const Vector2 & b) {
    return a.x*b.x+a.y*b.y;
}
inline Float cross (const Vector2 & a, const Vector2 & b) {
    return a.x*b.y-a.y*b.x;
}

inline bool isNan (const Vector2 & v) {
    return isNan(v.x) || isNan(v.y);
}


class Color {
public:
    Float r, g, b;
    inline Color () = default;
    inline explicit Color (Float l): r(l), g(l), b(l) {}
    inline Color (Float r, Float g, Float b): r(r), g(g), b(b) {}
    inline Color operator + (const Color & v) const {return Color(r+v.r, g+v.g, b+v.b);}
    inline Color operator - (const Color & v) const {return Color(r-v.r, g-v.g, b-v.b);}
    inline Color operator * (const Float & v) const {return Color(r*v, g*v, b*v);}
    inline Color operator / (const Float & v) const {return Color(r/v, g/v, b/v);}
    inline Color & operator += (const Color & v) {r += v.r, g += v.g, b += v.b; return *this;}
    inline Color & operator -= (const Color & v) {r -= v.r, g -= v.g; b -= v.b; return *this;}
    inline Color & operator *= (const Float & v) {r *= v, g *= v, b *= v; return * this;}
    inline Color & operator /= (const Float & v) {r /= v, g /= v, b /= v; return * this;}
};


#endif