#version 450
layout(location = 0) out vec4 fragColor;

in vec2 TexCoords;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAmbiant;
layout (binding = 3) uniform sampler2D gDiffuse;
layout (binding = 4) uniform sampler2D gSpecular;
layout (binding = 5) uniform sampler2D gAlbedo;
layout (binding = 6) uniform sampler2D ssao;


uniform vec3 lightPos;

struct Light {
    vec3 Position;
    vec3 Color;
    
    float Linear;
    float Quadratic;
};

uniform Light light;


void main()
{
/*
	ivec2 texCoords = ivec2(gl_FragCoord.xy);
	vec3 FragPos = texelFetch(gPosition,ivec2(texCoords),0).xyz;
    vec3 Normal = texelFetch(gNormal,ivec2(texCoords),0).xyz;    
    //vec3 ambient = texelFetch(gAmbiant,ivec2(texCoords),0).xyz;	
    vec3 Diffuse = texelFetch(gDiffuse,ivec2(texCoords),0).xyz;	
    vec3 specular = texelFetch(gSpecular,ivec2(texCoords),0).xyz;	
    float shininess = texelFetch(gSpecular,ivec2(texCoords),0).w;
    float AmbientOcclusion = texelFetch(ssao,ivec2(texCoords),0).w;


    // then calculate lighting as usual
    vec3 ambient = vec3(0.3 * Diffuse * AmbientOcclusion);
    vec3 lighting  = ambient; 
    vec3 viewDir  = normalize(-FragPos); // viewpos is (0.0.0)
    // diffuse
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * light.Color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(Normal, halfwayDir), 0.0), 8.0);
    //vec3 specular = light.Color * spec;
    // attenuation
    float distance = length(light.Position - FragPos);
    float attenuation = 1.0 / (1.0 + light.Linear * distance + light.Quadratic * distance * distance);
    diffuse *= attenuation;
    specular *= attenuation;
    lighting += diffuse + specular;

    fragColor = vec4(lighting, 1.0);
    */

	
	ivec2 texCoords = ivec2(gl_FragCoord.xy);
	vec3 position = texelFetch(gPosition,ivec2(texCoords),0).xyz;
    vec3 normal = texelFetch(gNormal,ivec2(texCoords),0).xyz;    
    vec3 ambient = texelFetch(gAmbiant,ivec2(texCoords),0).xyz;	
    vec3 diffuse = texelFetch(gDiffuse,ivec2(texCoords),0).xyz;	
    vec3 specular = texelFetch(gSpecular,ivec2(texCoords),0).xyz;	
    float shininess = texelFetch(gSpecular,ivec2(texCoords),0).w;
    float AmbientOcclusion = texture(ssao,ivec2(texCoords)).r;
    
     vec3 ambientWithAO = vec3(AmbientOcclusion * ambient);
     
	vec3 viewDir = normalize( - position.xyz);
	vec3 lightDir = normalize(position.xyz - lightPos);

	vec3 H = normalize(viewDir - lightDir);

	float cosThetaPowShininess = 0;

	cosThetaPowShininess = pow(max(dot(normal,H),0.0),shininess) ;

    vec3 specularLighting = specular.xxx * cosThetaPowShininess ;
	float cosTheta = max(dot(normalize(normal),normalize(lightPos-position.xyz)),0.f);
	vec3 diffuseLight = diffuse * cosTheta ;
    //remettre l'AO
	vec3 result =  ambientWithAO + diffuseLight.xyz + specularLighting;

	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	float facteurAtenuation = 1/dot(lightPos,position)*dot(lightPos,position); 
	
	 result = vec3(pow(result.x,1.5),pow(result.y,1.5),pow(result.z,1.5));
	// result *= facteurAtenuation;

	result = ((result*(a*result+b))/(result*(c*result+d)+e));
	fragColor =  vec4(result* AmbientOcclusion,1)  ;

	//fragColor =  vec4(AmbientOcclusion,AmbientOcclusion,AmbientOcclusion,1)  ;
    
    
}
