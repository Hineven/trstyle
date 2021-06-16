#version 430 core

layout(local_size_x = 16, local_size_y = 16) in;

uniform float v[4];

layout (rgb32f, binding = 0) uniform image2D input_image;
layout (rgb32f, binding = 0) uniform image2D output_image;

shared vec4 scanline[16][16];

void main(){
    
}