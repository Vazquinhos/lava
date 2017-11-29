#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform LightUniformBuffer
{
    vec3 position;
	vec3 direction;
	vec2 rangeAttenuation;
    vec3 color;
	float intensity;
} ubo;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragWorldPos;

layout(location = 0) out vec4 outColor;

vec3 sLightPosition = vec3(0.0,0.0,10.0);

void main()
{
	vec3 lightDir = normalize(ubo.position- vec3(fragWorldPos));
	float NDotL = max(dot(fragColor,lightDir), 0.0);
	//outColor = fragWorldPos;
    //outColor = vec4(fragColor,1.0)*0.5 +0.5;
	//outColor = vec4(1,0,0,0) * NDotL;
	outColor = texture(texSampler, fragTexCoord) * NDotL;
}