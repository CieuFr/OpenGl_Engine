#version 450

layout( location = 0 ) out vec4 fragColor;
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

	if(uHasNormalMap){
		textureNormalOrProgramNormal = texture(uNormalMap,texCoords).xyz;
		textureNormalOrProgramNormal = normalize(textureNormalOrProgramNormal * 2.0 - 1.0);   
	} else {
		textureNormalOrProgramNormal = normal;
	}

	float normalDirection;
	normalDirection = dot(textureNormalOrProgramNormal,viewDir);

	vec3 normalAfterCheck;

	if(normalDirection < 0) {
		normalAfterCheck = -textureNormalOrProgramNormal;
	} else {
		normalAfterCheck = textureNormalOrProgramNormal;
	}

	


	vec3 H = normalize(viewDir - lightDir);



	float cosThetaPowShininess = 0;

	if(uHasShininessMap){
		 cosThetaPowShininess = pow(max(dot(normalAfterCheck,H),0.0),vec3(texture(uShininessMap,texCoords)).x) ;
	} else {
		 cosThetaPowShininess = pow(max(dot(normalAfterCheck,H),0.0),shininess) ;
	}



	vec3 specularLighting = vec3(0,0,0); 
	if(uHasSpecularMap){
		 specularLighting = vec3(texture(uSpecularMap,texCoords)).xxx * cosThetaPowShininess ;
	} else {
		 specularLighting = specular * cosThetaPowShininess ;
	}



	float cosTheta = max(dot(normalize(normalAfterCheck),normalize(TlightPos-position.xyz)),0.f);

	vec3 diffuseLight = vec3(0.0,0.0,0.0);

	
	if(uHasDiffuseMap){
		 diffuseLight = vec3(texture(uDiffuseMap,texCoords)) * cosTheta ;
	} else {
		 diffuseLight = diffuse * cosTheta ;
	}
	
	vec3 result = vec3(0.0,0.0,0.0);
	if(uHasAmbientMap){
		result =  (vec3(texture(uAmbientMap,texCoords)) + diffuseLight + specularLighting);
	} else {
		result =  (ambient + diffuseLight + specularLighting);
	}
	

	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	float facteurAtenuation = 1/dot(TlightPos,Tposition)*dot(TlightPos,Tposition);

	 result = vec3(pow(result.x,1.5),pow(result.y,1.5),pow(result.z,1.5));
	// result *= facteurAtenuation;

	result = ((result*(a*result+b))/(result*(c*result+d)+e));
	fragColor =  vec4(result,1)  ;

	//Transparence
	//texture(uDiffuseMap,texCoords).w
}
