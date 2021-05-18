/*
 * Global configuration and shared data. Insert your configuration here
 * if needed.
 * 
 * Copyright (c) 2021 QwQ Unlimited Company
 */
#ifndef _H_DATA
#define _H_DATA
#include "heads.hpp"
#include <png++/png.hpp>
#include <png.h>
namespace Data {

// Parallel calculation maximun threads
// Change it to a proper number if your PC has more/less cores
constexpr int ncores = 8;

// Stepsize for differential measurement in pixels
constexpr Float delta = 1;
// Initial stepsize for gradient descent
constexpr Float h = 1e-6;
// Maximum stepsize for gradient descent
constexpr Float smax = 3;
// laaaaaaaambda (amplifier of the restriction factor, refer to
// the original paper or my notes for details)
constexpr Float lambda = 1e3;

// Triangle split threshould
// A new vertice will be placed at the cendroid of the triangle
// if it's energy density exceeds this value.
constexpr Float split_density_threshold = 40;
// Triangle split threshould
// The triangle will NOT split if its energy is lower than this value
constexpr Float split_energy_lower_threshold = 1e2;

// Minimum area of a triangle, smaller triangles will be collapsed
constexpr Float collapse_area_threshold = 150;

// Needed relative energy desend for a flip operation.
constexpr Float flip_threshould = 0.02;

// Interval of a maintain operation.
constexpr int split_interval = 11;
// Interval of a maintain operation.
constexpr int collapse_interval = 2;
// Interval of a maintain operation.
constexpr int flip_interval = 5;

// The input image data is shared all over the program
// Image width
extern int image_width ;
// Image height
extern int image_height;
// Channels of the image file. We only support 3-channel images.
constexpr int image_nchan = 3;
// image_width * image_height * image_nchan
extern unsigned char * image;
// Output buffer. Some render result will also be displayed here
// 这个结构在使用GPU加速后应该会被改掉，不然绘制太慢了
// 不过那暂时不是我该考虑的事情，嘿嘿嘿 -- Hineven
extern Float * image_output;

// Rendering style of a single triangle
constexpr enum RenderingStyle {
    // Constant coloring triangles
    F_RENDER_CONSTANT = 1,
    // Linear interpolate triangle vertice colors
    F_RENDER_LINEAR = 2
} style = F_RENDER_CONSTANT;


// Note: I only support 3 channel RGB/BGR image.
inline void loadImage (
    std::string filename // Image file name 
    ) {
    std::cout << "Loading image file: " << filename << std::endl;
    // Using libpng to load image from a png file
    // (Code modiefied from an example of the libpng source code)
    // Control structure of libpng
    png_image pimage;
    memset(&pimage, 0, sizeof(pimage));
    pimage.version = PNG_IMAGE_VERSION;
    if (png_image_begin_read_from_file(&pimage, filename.c_str()) == 0) 
        fatal("Failed to load image file: "+filename);
    if(pimage.format|PNG_FORMAT_FLAG_ALPHA)
        std :: cout << "Ignoring alpha channel.";
    pimage.format = PNG_FORMAT_RGB;
    // Allocate memory for image data.
    image = (unsigned char*)malloc(PNG_IMAGE_SIZE(pimage));
    if (image != nullptr &&
        png_image_finish_read(&pimage, NULL, image,
        0, NULL) != 0) {
        image_width = pimage.width;
        image_height = pimage.height;
        png_image_free(&pimage);
        image_output = new Float[image_width*image_height*image_nchan];
    } else {
        if (image == nullptr) png_image_free(&pimage);
        else free(image), image = nullptr;
        if (png_image_begin_read_from_file(&pimage, filename.c_str()) == 0) 
            fatal("Failed to load image file: " + filename);
    }
}

inline Color pixelColor (Float * arr, int x, int y) {
    int off = image_nchan*(y*image_width+x);
    return Color(arr[off], arr[off+1], arr[off+2]);
}
inline Color pixelColor (unsigned char * arr, int x, int y) {
    int off = image_nchan*(y*image_width+x);
    return Color(arr[off], arr[off+1], arr[off+2]);
}

inline void pixelSetColor (Float * arr, int x, int y, const Color & c) {
    int off = image_nchan*(y*image_width+x);
    arr[off] = c.r;
    arr[off+1] = c.g;
    arr[off+2] = c.b;
}
inline void pixelSetColor (unsigned char * arr, int x, int y, const Color & c) {
    int off = image_nchan*(y*image_width+x);
    arr[off] = c.r;
    arr[off+1] = c.g;
    arr[off+2] = c.b;
}

inline Vector2 clamp (const Vector2 & a) {
    return Vector2(
        std::min(std::max(0.0f, a.x), (Float)image_width-epsilon),
        std::min(std::max(0.0f, a.y), (Float)image_height-epsilon)
    );
}

}

#endif