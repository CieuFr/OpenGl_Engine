#version 450

layout( location = 0 ) out vec4 fragColor;

in vec3 normal;
in vec4 position;

uniform vec3 ambient;
uniform float luminosity;
uniform vec3 diffuse;
uniform vec3 lightPos;


void main()
{

	float diff = max(dot(normal,normalize(lightPos-position.xyz)),0.f);
	vec3 diffuseLight = diffuse * diff ;

	vec3 result = ambient + diffuseLight;


	fragColor =  vec4(result,1.0)  ;
}
