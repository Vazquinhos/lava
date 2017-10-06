#version 400
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 inUv;
layout (location = 0) out vec2 outUv;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

out gl_PerVertex {
    highp vec4 gl_Position;
};

void main()
{
	gl_Position = ubo.proj * ubo.view * ubo.model * vec4(pos, 1.0);
    outUv = inUv;
}
