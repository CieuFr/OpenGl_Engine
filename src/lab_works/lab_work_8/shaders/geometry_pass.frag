#version 450

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAmbiant;
layout (location = 3) out vec3 gDiffuse;
layout (location = 4) out vec4 gSpecular;
layout (location = 5) out vec3 gAlbedo;


layout (binding = 1) uniform sampler2D uDiffuseMap;
layout (binding = 2) uniform sampler2D uAmbientMap;
layout (binding = 3) uniform sampler2D uSpecularMap;
layout (binding = 4) uniform sampler2D uShininessMap;
layout (binding = 5) uniform sampler2D uNormalMap;

in vec3 normal;
in vec4 position;
in vec2 texCoords;

uniform vec3 ambient;
uniform float luminosity;
uniform vec3 diffuse;

uniform vec3 cameraPos;
uniform vec3 specular;
uniform float shininess;

uniform bool uHasDiffuseMap;
uniform bool uHasShininessMap;
uniform bool uHasSpecularMap;
uniform bool uHasAmbientMap;
uniform bool uHasNormalMap;

void main()
{
     // TEXTURES
	 if(texture(uDiffuseMap,texCoords).w <0.5) discard;

	 vec3 afterCheckLightPos;
	 vec3 afterCheckPosition;

	 vec3 textureNormalOrProgramNormal;

	if(uHasNormalMap){
		textureNormalOrProgramNormal = texture(uNormalMap,texCoords).xyz; 
	} else {
		textureNormalOrProgramNormal =  normalize(normal) ;

	}

	afterCheckPosition = position.xyz;

	vec3 viewDir = normalize( - afterCheckPosition);
	vec3 lightDir = normalize(afterCheckPosition - afterCheckLightPos);


	// FLIP NORMALS
	float normalDirection;
	normalDirection = dot(textureNormalOrProgramNormal,viewDir);
	vec3 normalAfterCheck;
	if(normalDirection < 0) {
		normalAfterCheck = -textureNormalOrProgramNormal;
	} else {
		normalAfterCheck = textureNormalOrProgramNormal;
	}


	float afterCheckShininess = 0;
	if(uHasShininessMap){
		afterCheckShininess = vec3(texture(uShininessMap,texCoords)).x;
	} else {
		afterCheckShininess = shininess;
	}


	vec3 afterCheckSpecular = vec3(0.0,0.0,0.0);
	if(uHasSpecularMap){
		 afterCheckSpecular = vec3(texture(uSpecularMap,texCoords)).xxx  ;
	} else {
		afterCheckSpecular = specular;
	}

	vec3 afterCheckDiffuse = vec3(0.0,0.0,0.0);
	if(uHasDiffuseMap){
		 afterCheckDiffuse = vec3(texture(uDiffuseMap,texCoords))  ;
	} else {
		 afterCheckDiffuse = diffuse;
	}
	

	vec3 afterCheckAmbient = vec3(0.0,0.0,0.0);
	if(uHasAmbientMap){
		afterCheckAmbient = vec3(texture(uAmbientMap,texCoords));
	} else {
		afterCheckAmbient = ambient;
	}


    gPosition = afterCheckPosition;
 	gNormal = normalize(normalAfterCheck);
	gAmbiant = afterCheckAmbient;
	gDiffuse = afterCheckDiffuse;
	gSpecular =vec4(afterCheckSpecular,shininess);
	gAlbedo.rgb = vec3(0.95);


}
