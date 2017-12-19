#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec4 fragWorldPos;

out gl_PerVertex {
    vec4 gl_Position;
};

void main()
{
	fragWorldPos = ubo.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPosition, 1.0);
	gl_Position.y = -1 * gl_Position.y;
    fragColor = mat3(transpose(inverse(ubo.model))) * inColor;
    fragTexCoord = inTexCoord;
	fragTexCoord.y = 1-fragTexCoord.y;
}