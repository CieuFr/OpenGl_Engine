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
out vec3 Tposition;
out vec2 texCoords;
out vec3 TlightPos;

void main()
{
	gl_Position = uMVPMatrix * vec4( aVertexPosition, 1.f );
	position = MVMatrix *  vec4( aVertexPosition, 1.f );
	normal =  normalMatrix * aVertexNormal;
	texCoords = aVertexTexCoords;
	normal = normalize(normal);
	vec3 T = normalize(vec3(MVMatrix * vec4(aVertexTangent,0.0)));
    vec3 B = normalize(vec3(MVMatrix * vec4(aVertexBitagent,0.0)));
    vec3 N = normalize(vec3(MVMatrix * vec4(aVertexNormal,0.0)));
    mat3 inv_TBN = transpose(mat3(T, B, N));
	TlightPos =  inv_TBN * lightPos;
	Tposition = inv_TBN * position.xyz ;

}
