#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include "utils/CReader.h"
#include "utils/CTimer.h"

#define WIDTH    16    //data block width
#define HEIGHT    16    //data block height

using namespace std;

void initGLSL(GLenum type);

void initFBO(unsigned unWidth, unsigned unHeight);

void initGLUT(int argc, char **argv);

void createTextures(void);

void setupTexture(const GLuint texID);

void performCompute(const GLuint inputTexID, const GLuint outputTexID);

void transferFromTexture(float *data);

void transferToTexture(float *data, GLuint texID);

// 纹理标识
GLuint outputTexID;
GLuint intermediateTexID;
GLuint inputTexID;

// GLSL 变量
GLuint glslProgram;
GLuint fragmentShader;

GLenum type = GL_FLOAT;

// FBO 标识
GLuint fb;

// 提供GL环境
GLuint glutWindowHandle;

struct structTextureParameters {
    GLenum texTarget;
    GLenum texInternalFormat;
    GLenum texFormat;
    char *shader_source;
} textureParameters;

float *pfInput;            //输入数据 
unsigned unWidth = (unsigned) WIDTH;
unsigned unHeight = (unsigned) HEIGHT;
unsigned unSize = unWidth * unHeight;

GLfloat v[500];

int main(int argc, char **argv) {
    int i;
    // create test data
    unsigned unNoData = 4 * unSize;        //total number of Data
    pfInput = new float[unNoData];
    float *pfOutput = new float[unNoData];
    for (i = 0; i < unNoData; i++) pfInput[i] = i * 0.001f;
    for (i = 0; i < 500; i++) {
        v[i] = i;
    }

    // create variables for GL
    textureParameters.texTarget = GL_TEXTURE_2D;
    textureParameters.texInternalFormat = GL_RGBA32F;
    textureParameters.texFormat = GL_RGBA;
    CReader reader;

    // 初始化 glut  glew
    initGLUT(argc, argv);
    glewInit();
    // 初始化FBO
    initFBO(unWidth, unHeight);

    createTextures();

    char c_convolution[] = "../convolution.cs";
    textureParameters.shader_source = reader.textFileRead(c_convolution);
    initGLSL(GL_COMPUTE_SHADER);
    performCompute(inputTexID, intermediateTexID);

    performCompute(intermediateTexID, outputTexID);

    // get GPU results
    transferFromTexture(pfOutput);

    for (int i = 0; i < unNoData; i++) {
        cout << "input:" << pfInput[i] << " output:" << pfOutput[i] << endl;
        
    }

    // clean up
    glDetachShader(glslProgram, fragmentShader);
    glDeleteShader(fragmentShader);
    glDeleteProgram(glslProgram);
    glDeleteFramebuffersEXT(1, &fb);
    glDeleteTextures(1, &inputTexID);
    glDeleteTextures(1, &outputTexID);
    glutDestroyWindow(glutWindowHandle);

    // exit
    delete pfInput;
    delete pfOutput;
    return EXIT_SUCCESS;
}

/**
 * Set up GLUT. The window is created for a valid GL environment.
 */
void initGLUT(int argc, char **argv) {
    glewExperimental = GL_TRUE;
    glutInit(&argc, argv);
    glutWindowHandle = glutCreateWindow("GPGPU Tutorial");
}

/**
 * Off-screen Rendering.
 */
void initFBO(unsigned unWidth, unsigned unHeight) {
    // create FBO (off-screen framebuffer)
    glGenFramebuffers(1, &fb);
    // bind offscreen framebuffer (that is, skip the window-specific render target)
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

}

/**
 * Set up the GLSL runtime and creates shader.
 */
void initGLSL(GLenum type) {
    // create program object
    glslProgram = glCreateProgram();
    // create shader object (fragment shader)
    fragmentShader = glCreateShader(type);
    // set source for shader
    const GLchar *source = textureParameters.shader_source;
    glShaderSource(fragmentShader, 1, &source, nullptr);
    // compile shader
    glCompileShader(fragmentShader);

    // attach shader to program
    glAttachShader(glslProgram, fragmentShader);
    // link into full program, use fixed function vertex shader.
    // you can also link a pass-through vertex shader.
    glLinkProgram(glslProgram);

}
c
/**
 * create textures and set proper viewport etc.
 */
void createTextures() {
    // 创建两个纹理
    // y 输出; x 输入.
    glGenTextures(1, &outputTexID);
    glGenTextures(1, &intermediateTexID);
    glGenTextures(1, &inputTexID);
    // set up textures
    setupTexture(outputTexID);
    setupTexture(intermediateTexID);
    setupTexture(inputTexID);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureParameters.texTarget, inputTexID, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, textureParameters.texTarget, intermediateTexID, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, textureParameters.texTarget, outputTexID, 0);
    transferToTexture(pfInput, inputTexID);
    // set texenv mode
    //glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

/**
 * Sets up a floating point texture with the NEAREST filtering.
 */
void setupTexture(const GLuint texID) {
    // make active and bind
    glBindTexture(textureParameters.texTarget, texID);
    glTexStorage2D(GL_TEXTURE_2D, 8, GL_RGBA32F, 16, 16);
    // turn off filtering and wrap modes
    glTexParameteri(textureParameters.texTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(textureParameters.texTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(textureParameters.texTarget, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(textureParameters.texTarget, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // define texture with floating point format
    glTexImage2D(textureParameters.texTarget, 0, textureParameters.texInternalFormat, unWidth, unHeight, 0,
                 textureParameters.texFormat, type, nullptr);
}

void performCompute(const GLuint inputTexID, const GLuint outputTexID) {

    
    // enable GLSL program
    glUseProgram(glslProgram);
    // enable the read-only texture x
    //glActiveTexture(GL_TEXTURE0);
    glUniform1fv(glGetUniformLocation(glslProgram, "v"), 4, v);

    // Synchronize for the timing reason.
    glFinish();

    CTimer timer;
    long lTime;
    timer.reset();

    glBindImageTexture(0, inputTexID, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glBindImageTexture(1, outputTexID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glDispatchCompute(1, 1, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    glFinish();
    lTime = timer.getTime();
    cout << "Time elapsed: " << lTime << " ms." << endl;
}

/**
 * Transfers data from currently texture to host memory.
 */
void transferFromTexture(float *data) {
    glReadBuffer(GL_COLOR_ATTACHMENT1);
    glReadPixels(0, 0, unWidth, unHeight, textureParameters.texFormat, type, data);
}

/**
 * Transfers data to texture. Notice the difference between ATI and NVIDIA.
 */
void transferToTexture(float *data, GLuint texID) {
    // 绑定 为帧缓存， 以后的着色器操作均会在此纹理上进行
    glBindTexture(textureParameters.texTarget, texID);
    glTexSubImage2D(textureParameters.texTarget, 0, 0, 0, unWidth, unHeight, textureParameters.texFormat, type, data);
}
