#version 450
layout( location = 0 ) out vec4 fragColor;

//Position en tangent space

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAmbiant;
layout (binding = 3) uniform sampler2D gDiffuse;
layout (binding = 4) uniform sampler2D gSpecular;

 in vec3 TlightPos;
 in vec2 TexCoords;


void main()
{
//	ivec2 texCoords = ivec2(gl_FragCoord.xy);
//	vec3 normal = texelFetch(gNormal,texCoords,0).xyz;
//	vec3 position = texelFetch(gPosition,texCoords,0).xyz;
//	vec3 ambient = texelFetch(gAmbiant,texCoords,0).xyz;
//	vec3 diffuse = texelFetch(gDiffuse,texCoords,0).xyz;
//	vec3 specular = texelFetch(gSpecular,texCoords,0).xyz;
//	float shininess = texelFetch(gSpecular,texCoords,0).w;
	

	ivec2 texCoords = ivec2(gl_FragCoord.xy);
	vec3 normal = texelFetch(gNormal,ivec2(TexCoords),0).xyz;     // TEXTURES
	vec3 position = texelFetch(gPosition,ivec2(TexCoords),0).xyz;
	vec3 ambient = texelFetch(gAmbiant,ivec2(TexCoords),0).xyz;	//BLIN PHONG
	vec3 diffuse = texelFetch(gDiffuse,ivec2(TexCoords),0).xyz;	
	vec3 specular = texelFetch(gSpecular,ivec2(TexCoords),0).xyz;	
	float shininess = texelFetch(gSpecular,ivec2(TexCoords),0).w;
	

	vec3 viewDir = normalize( - position.xyz);
	vec3 lightDir = normalize(position.xyz - TlightPos);

	vec3 H = normalize(viewDir - lightDir);

	float cosThetaPowShininess = 0;



	cosThetaPowShininess = pow(max(dot(normal,H),0.0),shininess) ;

   vec3 specularLighting = specular.xxx * cosThetaPowShininess ;




	float cosTheta = max(dot(normalize(normal),normalize(TlightPos-position.xyz)),0.f);

	 vec3 diffuseLight = diffuse * cosTheta ;
	
	

	vec3 result =  (ambient + diffuseLight.xyz + specularLighting);


	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	float facteurAtenuation = 1/dot(TlightPos,position)*dot(TlightPos,position);

	// result = vec3(pow(result.x,1.5),pow(result.y,1.5),pow(result.z,1.5));
	// result *= facteurAtenuation;

	//result = ((result*(a*result+b))/(result*(c*result+d)+e));
	fragColor =  vec4(result,1)  ;

	//Transparence
	//texture(uDiffuseMap,texCoords).w
}
