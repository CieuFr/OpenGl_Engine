#version 450

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAmbiant;
layout (location = 3) out vec3 gDiffuse;
layout (location = 4) out vec4 gSpecular;

layout (binding = 1) uniform sampler2D uDiffuseMap;
layout (binding = 2) uniform sampler2D uAmbientMap;
layout (binding = 3) uniform sampler2D uSpecularMap;
layout (binding = 4) uniform sampler2D uShininessMap;
layout (binding = 5) uniform sampler2D uNormalMap;

in vec3 normal;
in vec4 position;
in vec2 texCoords;
in vec3 TlightPos;
in vec3 Tposition;

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

	//BLIN PHONG
	vec3 viewDir = normalize( - position.xyz);
	vec3 lightDir = normalize(position.xyz - TlightPos);

	vec3 textureNormalOrProgramNormal;

	// NORMAL MAP 
	if(uHasNormalMap){
		textureNormalOrProgramNormal = texture(uNormalMap,texCoords).xyz;
		textureNormalOrProgramNormal = normalize(textureNormalOrProgramNormal * 2.0 - 1.0);   
	} else {
		textureNormalOrProgramNormal = normal;
	}


	// FLIP NORMALS
	float normalDirection;
	normalDirection = dot(textureNormalOrProgramNormal,viewDir);
	vec3 normalAfterCheck;

	if(normalDirection < 0) {
		normalAfterCheck = -textureNormalOrProgramNormal;
	} else {
		normalAfterCheck = textureNormalOrProgramNormal;
	}

	

	// COMPUTE SHININESS 

	vec3 H = normalize(viewDir - lightDir);

	float afterCheckShininess = 0;
	float cosThetaPowShininess = 0;
	if(uHasShininessMap){
		afterCheckShininess = vec3(texture(uShininessMap,texCoords)).x;
	} else {
		afterCheckShininess = shininess;
	}
	cosThetaPowShininess = pow(max(dot(normalAfterCheck,H),0.0),afterCheckShininess);

	// COMPUTE SPECULAR 
	vec3 afterCheckSpecular = vec3(0.0,0.0,0.0);
	vec3 specularLighting = vec3(0,0,0); 
	if(uHasSpecularMap){
		 afterCheckSpecular = vec3(texture(uSpecularMap,texCoords)).xxx  ;
	} else {
		afterCheckSpecular = specular;
	}
	 specularLighting = afterCheckSpecular * cosThetaPowShininess ;


	// COMPUTE DIFFUSE 

	float cosTheta = max(dot(normalize(normalAfterCheck),normalize(TlightPos-position.xyz)),0.f);
	
	vec3 diffuseLighting = vec3(0.0,0.0,0.0);
	vec3 afterCheckDiffuse = vec3(0.0,0.0,0.0);
	if(uHasDiffuseMap){
		 afterCheckDiffuse = vec3(texture(uDiffuseMap,texCoords))  ;
	} else {
		 afterCheckDiffuse = diffuse;
	}
	 diffuseLighting = afterCheckDiffuse * cosTheta;
	
	// LIGHT ATENUATION
	
	float distance = length(TlightPos -Tposition);
	float facteurAtenuation = 1/(1.0f + 0.14f * distance + 0.07f * distance * distance) ;
	
	vec3 afterCheckAmbient = vec3(0.0,0.0,0.0);

	// COMPUTE AMBIANT
	if(uHasAmbientMap){
		afterCheckAmbient = vec3(texture(uAmbientMap,texCoords));
	} else {
		afterCheckAmbient = ambient;
	}

	vec3 result = vec3(0.0,0.0,0.0);
	result =  afterCheckAmbient + diffuseLighting + specularLighting;
	
	
	// ACES TONE MAPPING
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	result = vec3(pow(result.x,1.5),pow(result.y,1.5),pow(result.z,1.5));
	// result *= facteurAtenuation;
	result = ((result*(a*result+b))/(result*(c*result+d)+e));
	

	
    gPosition = position.xyz;
 	gNormal = normalize(normalAfterCheck);
	gAmbiant = afterCheckAmbient;
	gDiffuse = afterCheckDiffuse;
	gSpecular =vec4(afterCheckSpecular,shininess);

	//Transparence
	//texture(uDiffuseMap,texCoords).w
}
