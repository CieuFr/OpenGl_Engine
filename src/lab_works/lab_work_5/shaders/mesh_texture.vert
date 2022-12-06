#version 450

layout( location = 0 ) in vec3 aVertexPosition;
layout( location = 1 ) in vec3 aVertexNormal;
layout( location = 2 ) in vec2 aVertexTexCoords;
layout( location = 3 ) in vec3 aVertexTangent;
layout( location = 4 ) in vec3 aVertexBitagent;

uniform mat4 uMVPMatrix; // Projection * View * Model
uniform mat3 normalMatrix;
uniform mat4 MVMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPos;


out vec3 normal;
out vec4 position;
out vec3 tangentPosition;
out vec2 texCoords;
out vec3 tangentLightPos;
out vec3 viewLightPos;




void main()
{
	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
	position = MVMatrix *  vec4( aVertexPosition, 1.f );
	texCoords = aVertexTexCoords;
	vec3 T = normalize(vec3(normalMatrix * aVertexTangent));
    vec3 N = normalize(vec3(normalMatrix * aVertexNormal ));
	T = normalize ( T - dot (T , N ) * N );
	vec3 B = cross (N , T );
    mat3 inv_TBN = transpose(mat3(T, B, N));
	tangentLightPos =  inv_TBN * lightPos;
	tangentPosition = inv_TBN * position.xyz ;
	normal = normalMatrix * aVertexNormal;
	viewLightPos = lightPos;
}
