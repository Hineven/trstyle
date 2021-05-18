//#include <OpenCV/opencv>
// testing opencv avaliability

#include "base/heads.hpp"

void render (Vector2 a, Vector2 b, Vector2 c, Float * buffer) {
    int ymn, ymx;
    ymn = std::max(0, (int)floor(std::min(a.y, std::min(b.y, c.y))));
    ymx = std::min(100-1, (int)ceil(std::max(a.y, std::max(b.y, c.y))));
    Color sum(0);
    int npixels = 0;
    // Loop over the triangle area
    for(int y = ymn; y<=ymx; y++) {
        int xmn = 0, xmx = 99;
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
        printf("y:%d, xmn:%d, xmx:%d\n", y, xmn, xmx);
        // Boundaries found, loop over each valid pixel
        for(int x = xmn; x<=xmx; x++) {
            assert(x >= 0 && x < 100);
            assert(y >= 0 && y < 100);
            npixels ++;
            printf("at: %d %d\n", x, y);
        }
    }
    printf("%d\n", npixels);
}

int main () {
    render({0, 0}, {30, 11}, {7, 44}, nullptr);
    printf("%f\n", cross({30, 11}, {7, 44})/2);
}