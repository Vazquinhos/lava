#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragNormal;

layout(location = 0) out vec4 outColor;

vec4 lightColor = vec4(1,1,1,1);

vec4 DoDiffuse( float3 L, float3 N )
{
    float NdotL = max( 0, dot( N, L ) );
    return  * NdotL;
}

void main() {
    outColor = vec4(fragNormal, 1.0);
}