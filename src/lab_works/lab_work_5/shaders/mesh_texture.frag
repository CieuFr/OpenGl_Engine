#version 450

layout( location = 0 ) out vec4 fragColor;
layout (binding = 1) uniform sampler2D uDiffuseMap;

in vec3 normal;
in vec4 position;
in vec2 texCoords;

uniform vec3 ambient;
uniform float luminosity;
uniform vec3 diffuse;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 specular;
uniform float shininess;
uniform bool uHasDiffuseMap;



void main()
{
     // TEXTURES

	//BLIN PHONG
	vec3 viewDir = normalize(cameraPos - position.xyz);
	vec3 lightDir = normalize(position.xyz - lightPos);

	float normalDirection = dot(normal,viewDir);

	vec3 normalAfterCheck;

	if(normalDirection < 0) {
		normalAfterCheck = -normal;
	} else {
		normalAfterCheck = normal;
	}

	


	vec3 H = normalize(viewDir - lightDir);

	vec3 spec =  pow(max(dot(normalAfterCheck,H),0.0),shininess) * specular;

	float cosTheta = max(dot(normalize(normalAfterCheck),normalize(lightPos-position.xyz)),0.f);

	vec3 diffuseLight = vec3(0.0,0.0,0.0);

	if(uHasDiffuseMap){
		 diffuseLight = vec3(texture(uDiffuseMap,texCoords)) * cosTheta ;
	} else {
		 diffuseLight = diffuse * cosTheta ;
	}
	


	vec3 result = (ambient + diffuseLight + spec);



	fragColor =  vec4(result,1.0)  ;
}
