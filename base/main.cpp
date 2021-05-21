// 这是一个简单的Demo，展示了一下梯度下降模块的使用方法
// 大家可以用它先看看效果。

#include "data.hpp"
#include "stylization.hpp"
// 这个头文件需要安装OpenGL以及GLFW库，在Windows下需要特别配置
#include <GLFW/glfw3.h>
// 调试用的
#ifndef _WIN32
#include <unistd.h>
#endif

// 像素放大系数，如果用高分屏可以把这个东西开成2以上。
const int display_scaler = 2;

GLubyte img[1920*1280][3];


int main(int argc, char**argv)
{
    srand(10000);

    if(argc < 2) fatal("Useage: style <PNG file name> [maximum energy density] [maximum triangles]");
    if(argc >= 3) sscanf(argv[2], "%f", &Data::split_density_threshold);
    if(argc >= 4) sscanf(argv[3], "%d", &Data::max_triangles);
    Data::loadImage(argv[1]);
    Stylization::initializeSimple();


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
    while (!glfwWindowShouldClose(window))
    {
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
    getchar();
    getchar();
    getchar();
    getchar();
    getchar();
    glfwTerminate();

    return 0;
}
