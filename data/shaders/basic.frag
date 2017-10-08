#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (location = 0) in vec2 uv;
layout (location = 0) out vec4 outputColor;

layout(set = 0, binding = 0) uniform ColorUniform {
    vec3 color;
} material;


void main() {
   outputColor = vec4 (1,0,0,1);
}