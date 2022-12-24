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
layout (binding = 7) uniform sampler2D shadowMap;


uniform vec3 lightPos;

uniform mat4 lightSpaceMatrix;

struct Light {
    vec3 Position;
    vec3 Color;
    float Linear;
    float Quadratic;
};

uniform Light light;

//LEARN OPENGL
float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{

	ivec2 texCoords = ivec2(gl_FragCoord.xy);
	vec3 position = texelFetch(gPosition,ivec2(texCoords),0).xyz;
    vec3 normal = texelFetch(gNormal,ivec2(texCoords),0).xyz;    
    //vec3 ambient = texelFetch(gAmbiant,ivec2(texCoords),0).xyz;	
    vec3 diffuse = texelFetch(gDiffuse,ivec2(texCoords),0).xyz;	
    vec3 specular = texelFetch(gSpecular,ivec2(texCoords),0).xyz;	
    float shininess = texelFetch(gSpecular,texCoords,0).w;
    float AmbientOcclusion = texelFetch(ssao,texCoords,0).x;
    
     //vec3 ambientWithAO = vec3(AmbientOcclusion * ambient);
     vec3 ambientOccluded = vec3(0.3 * diffuse * AmbientOcclusion);
	vec3 viewDir = normalize( - position.xyz);
	vec3 lightDir = normalize(position.xyz - lightPos);

	vec3 H = normalize(viewDir - lightDir);

	float cosThetaPowShininess = 0;

	cosThetaPowShininess = pow(max(dot(normal,H),0.0),shininess);

    vec3 specularLighting = specular.xxx * cosThetaPowShininess;
	float cosTheta = max(dot(normalize(normal),normalize(lightPos-position.xyz)),0.f);
	vec3 diffuseLight = diffuse * cosTheta;


     vec4 positionLightSpace = lightSpaceMatrix * vec4(position, 1.0);
     float shadow = ShadowCalculation(positionLightSpace);     
     
     
	vec3 result =  ambientOccluded + ((diffuseLight.xyz + specularLighting) * (1-shadow));

	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;

	float facteurAtenuation = 1/dot(lightPos,position)*dot(lightPos,position); 
	
	 result = vec3(pow(result.x,1.5),pow(result.y,1.5),pow(result.z,1.5));
	// result *= facteurAtenuation;

	result = ((result*(a*result+b))/(result*(c*result+d)+e));
	fragColor = vec4(diffuseLight * AmbientOcclusion  + specularLighting,1) ;
   //fragColor = vec4(AmbientOcclusion * 255,AmbientOcclusion * 255,AmbientOcclusion * 255, 1.);
	//fragColor =  vec4(AmbientOcclusion,AmbientOcclusion,AmbientOcclusion,1)  ;
    
    

// blinn-phong (in view-space)
//    vec3 ambient = vec3(0.3 * diffuse * AmbientOcclusion); // here we add occlusion factor
//    vec3 lighting  = ambient; 
//    vec3 viewDir  = normalize(-position); // viewpos is (0.0.0) in view-space
//    // diffuse
//    vec3 lightDir = normalize(lightPos - position);
//    vec3 diffuse2 = max(dot(normal, lightDir), 0.0) * diffuse * light.Color;
//    // specular
//    vec3 halfwayDir = normalize(lightDir + viewDir);  
//    float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);
//    vec3 specular2 = vec3( 0.2, 0.2, 0.7 ) * spec;
//    // attenuation
//    float dist = length(lightPos - position);
//    float attenuation = 1.0 / (1.0 + light.Linear * dist + light.Quadratic * dist * dist);
//    diffuse  *= attenuation;
//    specular2 *= attenuation;
//    lighting += diffuse + specular2;
//
//    fragColor = vec4(lighting, 1.0);



}
