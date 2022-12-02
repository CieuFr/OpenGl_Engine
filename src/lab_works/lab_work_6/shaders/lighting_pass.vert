#version 450

layout( location = 0 ) in vec2 aVertexPosition;
layout( location = 1 ) in vec2 aVertexTexCoords;



uniform mat4 MVMatrix;
uniform vec3 lightPos;

out vec2 texCoords;
out vec3 TlightPos;

void main()
{
	texCoords = aVertexTexCoords;
	
//	vec3 T = normalize(vec3(MVMatrix * vec4(aVertexTangent,0.0)));
//    vec3 N = normalize(vec3(MVMatrix * vec4(aVertexNormal,0.0)));
//	T = normalize ( T - dot (T , N ) * N );
//	vec3 B = cross (N , T );
//    mat3 inv_TBN = transpose(mat3(T, B, N));
//	TlightPos =  inv_TBN * lightPos;
//
	gl_Position = vec4(aVertexPosition,0,1);

}
