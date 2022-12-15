#version 450

layout(location = 0) out vec4 fragColor;

layout (binding = 0) uniform sampler2D depthMap;


void main()
{
	ivec2 texCoords = ivec2(gl_FragCoord.xy);
	vec3 position = texelFetch(depthMap,ivec2(texCoords),0).xyz;

	fragColor = vec4(vec3(position.x),1);

	fragColor = vec4(1,0,0,0);

}
