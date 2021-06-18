// 这是一个简单的Demo，展示了一下梯度下降模块的使用方法
// 大家可以用它先看看效果。
#include <stdio.h>
#include <stdlib.h>
#include "data.hpp"
#include "heads.hpp"
#include "stylization.hpp"
#include "Pretreatment.hpp"
// 这个头文件需要安装OpenGL以及GLFW库，在Windows下需要特别配置
#include <GL/glew.h>
#include <GL/glut.h>
#include <GLFW/glfw3.h>
// 调试用的
#ifndef _WIN32
#include <unistd.h>
#endif

// 像素放大系数，如果用高分屏可以把这个东西开成2以上。
const int display_scaler = 1;

GLubyte img[1920*1280][3];


struct _Tri {
    short a, b, c, v;
} tri[50000];

int main(int argc, char**argv)
{
    srand(10000);

    if(argc < 2) fatal("Useage: style <PNG file name> [maximum energy density] [maximum triangles] [initial vertices]");
    if(argc >= 3) sscanf(argv[2], "%f", &Data::split_density_threshold);
    if(argc >= 4) sscanf(argv[3], "%d", &Data::max_triangles);
    int n_vertices = 50;
    if(argc >= 4) sscanf(argv[4], "%d", &n_vertices);
    Data::loadImage(argv[1]);


    //Init::InitializeMain();
    //Stylization::initializeSimple();

if(n_vertices > 4) {
    Init::InitializeMain(n_vertices);
    //std::cerr<<"oooops!"<<std::endl;
    for(int i=0;i<Init::num;i++){
        tri[i].a = Init::trigs[i].v[0];
        tri[i].b = Init::trigs[i].v[1];
        tri[i].c = Init::trigs[i].v[2];
        Vector2 a, b, c;
        a = Init::verts[tri[i].a];
        b = Init::verts[tri[i].b];
        c = Init::verts[tri[i].c];
        if(cross(b-a, c-a) < 0) std::swap(tri[i].b, tri[i].c);
        tri[i].v = 0;
        //std::cerr<<tri[i].a<<" "<<tri[i].b<<" "<<tri[i].c<<std::endl;
    }
    for(int i=0;i<Init::n_init;i++){
        //std::cerr<<Init::verts[i].x<<" "<<Init::verts[i].y<<std::endl;
    }
    Stylization::initializeWith(Init::verts, Init::n_init, (Stylization::Triangle*)tri, Init::num);
}
else Stylization::initializeSimple();
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit())
        return 0;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(
        Data::image_width*display_scaler,
        Data::image_height*display_scaler,
        "Preview", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 0;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glViewport(0, 0, Data::image_width*display_scaler, Data::image_height*display_scaler);


    int iters = 0;
    int max_iters = 10000;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        ++iters;
        if(iters > max_iters) break ;
        Stylization::iterate();
        glRasterPos2d(-1, -1);
        for(int i = 0; i<Data::image_width; i++)
            for(int j = 0; j<Data::image_height; j++) {
                Color c = Data::pixelColor(Data::image_output, i, j);
                for(int dx = 0; dx<display_scaler; dx++)
                    for(int dy = 0; dy<display_scaler; dy++) {
                        int pos = ((Data::image_height-j-1)*display_scaler+dx)*Data::image_width*display_scaler;
                        pos += i*display_scaler+dy;
                        img[pos][0] = c.r;
                        img[pos][1] = c.g;
                        img[pos][2] = c.b;
                    }
            }
        glDrawPixels(
            Data::image_width*display_scaler,
            Data::image_height*display_scaler,
            GL_RGB, GL_UNSIGNED_BYTE, img
        );
#undef TOBYTE
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
        //sleep(1);

    }

    int l=(Data::image_width*3+3)/4*4;
    int bmi[]= {l*Data::image_height+54,0,54,40,Data::image_width,Data::image_height,1|3*8<<16,0,l*Data::image_height,0,0,100,0};
    FILE *fp = fopen("test.bmp","wb");
    fprintf(fp,"BM");
    fwrite(&bmi,52,1,fp);
    char img1[Data::image_height*Data::image_width*3];
    int num = 0;
    for(int i = 0; i<Data::image_width; i++)
        for(int j = 0; j<Data::image_height; j++) {
            img1[num++] = img[i*Data::image_width+j][2];
            img1[num++] = img[i*Data::image_width+j][1];
            img1[num++] = img[i*Data::image_width+j][0];
    }
    fwrite(img1,1,l*Data::image_height,fp);
    fclose(fp);
    //system("test.bmp");
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();
    glfwTerminate();

    return 0;
}
