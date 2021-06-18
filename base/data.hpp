/*
 * Global configuration and shared data. Insert your configuration here
 * if needed.
 * 这是一个配置文件，所有配置以及全局共享的量都在这里。
 *
 * Copyright (c) 2021 QwQ Unlimited Company
 */
#ifndef _H_DATA
#define _H_DATA
#include "heads.hpp"
#include <png++/png.hpp>
#include <png.h>
namespace Data {

// 最宏观，最重要的参数

// 三角形数目的严格上限
// Maximum number of triangles
extern int max_triangles;

// 目标剖分的每个三角形的能量密度上界
// Triangle split threshold
// A new vertice will be placed at the cendroid of the triangle
// if it's energy density exceeds this value.
extern Float split_density_threshold;

// 不那么重要的参数

// 预模糊核，防止迭代中由于梯度不光滑而来回抖动的情况
// （但貌似没什么效果...）
constexpr int blur_size = 2;
inline Float blurKernel (int dx, int dy) {
    int d = abs(dx)+abs(dy);
    if(d <= 1) return 0.10;
    if(d <= 2) return 0.0625;
    return 0;
}

// 核心数，请根据你电脑的（逻辑）核心数目调整
// Parallel calculation maximun threads
// Change it to a proper number if your PC has more/less cores
constexpr int ncores = 8;

constexpr Float learning_lambda = 0.3;

// 计算梯度时的微元的大小，单位是像素
// Stepsize for differential measurement in pixels
constexpr Float delta = 1;

// 梯度下降步长，太长会来回抖动，太短会龟速爬行
// Initial stepsize for gradient descent
constexpr Float h = 2e-1;

// 梯度下降的最大单次移动距离（单位是像素），防止梯度爆炸引起的瞬移
// Maximum stepsize for gradient descent
constexpr Float smax = 1.5;

// lambda，意义和论文中相同，由于我们这里色彩没有归一化，所以相对论文的参考值会大很多
// laaaaaaaambda (amplifier of the restriction factor, refer to
// the original paper or my notes for details)
constexpr Float lambda = 1;

// 当三角形的总能量小于这个值时，不能对这个三角形进行分裂，该规则会复写上一条规则
// 为0表示任何三角形都能被分裂
// Triangle split threshold
// The triangle will NOT split if its energy is lower than this value
constexpr Float split_energy_lower_threshold = 1000;

// 挑选三角形进行分裂时，这个值越大，那么越偏爱优先分裂总能量
// 更大的三角形，否然更偏爱分裂能量密度更大的三角形
// 0表示三角形总能量不影响分裂对象的挑选
// Split balance factor, the bigger the value is, the more possible
// bigger triangles with larger energy will be splited.
constexpr Float split_balance_factor = 0;

// 当三角形面积小于这个值时，这个三角形可以被坍缩
// Minimum area of a triangle, smaller triangles will be collapsed
constexpr Float collapse_area_threshold = 50;

// 用于判断坍缩是否使用边坍缩的方法
// 如果最短边长度小于这么多像素，才可能使用边坍缩的方法
constexpr Float collapse_edge_threshold = 10;

// 用于判断坍缩是否使用边坍缩的方法
// 如果最短边长度小于最长边的这个倍数，才可能使用边坍缩的方法
constexpr Float collapse_edge_r_threshold = 0.2;

// 当翻转边可以带来大于该值的相对能量衰减时，翻转，否然不翻转
// 0意味着只要能让能量下降，就一定翻转，越大，翻转的频率越低
// Needed relative energy desend for a flip operation.
constexpr Float flip_threshould = 0;

// 这么多次能量不下降就进一步细分
constexpr int split_stable = 8;
// 总能量至少相对下降这么多才算一次下降，0表示任何总能量减小都算能量下降
constexpr Float split_descend = 0.005;

// 每次分裂最多可以分裂多少三角形
// Maximum triangle splits performed in a maintain operation
constexpr int split_number = 64;

// 每这么多次迭代尝试一次坍缩（由于代码实现的缺陷，
// 一次坍缩最多只能坍缩一个三角形）
// Interval of a maintain operation.
constexpr int collapse_interval = 2;

// 每过这么多次迭代尝试一次翻转边，所有能翻转的边可以一起翻转，
// 如果对于一个三角形，有多条边可以翻转，那么随机选择
// Interval of a maintain operation.
constexpr int flip_interval = 7;

// 以下是程序运行期间使用的输入输出和全局数据
// The input image data is shared all over the program
// Image width
extern int image_width;
// Image height
extern int image_height;
// Channels of the image file. We only support 3-channel images.
constexpr int image_nchan = 3;
// image_width * image_height * image_nchan
extern unsigned char * image;
// Output buffer. Some render result will also be displayed here
// 这个结构在使用GPU加速后应该会被改掉，不然绘制太慢了
// 不过那暂时不是我该考虑的事情，嘿嘿嘿
extern Float * image_output;

// Rendering style of a single triangle
constexpr enum RenderingStyle {
    // Constant coloring triangles
    F_RENDER_CONSTANT = 1,
    // Linear interpolate triangle vertice colors
    F_RENDER_LINEAR = 2
} style = F_RENDER_CONSTANT;

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
        std :: cout << "Ignoring alpha channel." << std::endl;
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
        std :: cout << "Pre blurring..." << std::endl;
        unsigned char * t_buffer = new unsigned char [image_width*image_height*image_nchan];
        for(int i = 0; i<image_height; i++)
            for(int j = 0; j<image_width; j++) {
                Color col(0);
                for(int dx = -blur_size; dx<=blur_size; dx ++)
                    for(int dy = -blur_size; dy<=blur_size; dy ++) {
                        int px = std::max(0, std::min(image_width, j+dx));
                        int py = std::max(0, std::min(image_height, i+dy));
                        col += pixelColor(image, px, py)*blurKernel(dx, dy);
                    }
                pixelSetColor(t_buffer, j, i, col);
            }
        memcpy(image, t_buffer, image_width*image_height*image_nchan);
        delete [] t_buffer;
    } else {
        if (image == nullptr) png_image_free(&pimage);
        else free(image), image = nullptr;
        if (png_image_begin_read_from_file(&pimage, filename.c_str()) == 0) 
            fatal("Failed to load image file: " + filename);
    }
}

inline Vector2 clamp (const Vector2 & a) {
    return Vector2(
        std::min(std::max(0.0f, a.x), (Float)image_width),
        std::min(std::max(0.0f, a.y), (Float)image_height)
    );
}
inline Vector2 clampInside (const Vector2 & a) {
    return Vector2(
        std::min(std::max(epsilon, a.x), (Float)image_width-epsilon),
        std::min(std::max(epsilon, a.y), (Float)image_height-epsilon)
    );
}

}

#endif