#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 worldNormal;

layout(location = 0) out vec4 outColor;

vec4 lightColor = vec4(1,1,1,1);

void main()
{
	// normal is a 3D vector with xyz components; in -1..1
	// range. To display it as color, bring the range into 0..1
	// and put into red, green, blue components
    outColor = vec4(worldNormal*0.5 +0.5, 1.0);
	//outColor = vec4(0.0,1.0,0.0, 1.0);
}