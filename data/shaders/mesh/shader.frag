#version 450
#extension GL_ARB_separate_shader_objects : enable

precision mediump float;

layout(binding = 1) uniform LightUniformBuffer
{
    vec3 position;
	float beginRange;
	vec3 direction;
	float endRange;
	vec3 color;
	float intensity;
}
ubo;

layout(binding = 2) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragWorldPos;

layout(location = 0) out vec4 outColor;

void main()
{
	vec4 lightPos = vec4(ubo.position.x, ubo.position.y, ubo.position.z,1);
	vec4 lightDir = lightPos - fragWorldPos;
	float distance = length(lightDir);
    lightDir = lightDir / distance;

	float NDotL = max(dot(vec4(fragColor,1),lightDir), 0.0);

	vec4 diffuseContribuition = texture(texSampler, fragTexCoord) * NDotL * vec4(ubo.color,1);
	diffuseContribuition = clamp(diffuseContribuition, 0.0, 1.0);

	float attenuation = clamp( 1 - smoothstep( ubo.beginRange, ubo.endRange, distance ), 0.0, 1.0);

	outColor = texture(texSampler, fragTexCoord) * diffuseContribuition * ubo.intensity * attenuation;
}