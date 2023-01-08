#version 450
layout(location = 0) out vec4 fragColor;

in vec2 TexCoords;
in vec4 fragPosLight;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D gAmbiant;
layout (binding = 3) uniform sampler2D gDiffuse;
layout (binding = 4) uniform sampler2D gSpecular;
layout (binding = 5) uniform sampler2D gPosition2;
layout (binding = 6) uniform sampler2D ssao;
layout (binding = 7) uniform sampler2D shadowMap;

uniform vec3 lightPos;

uniform float lightPower;
uniform float lightAttenuationDistance;
uniform mat4 lightSpaceMatrix;
uniform mat4 worldToViewMatrix;


//LEARN OPENGL
float ShadowCalculation(vec4 fragPosLightSpace, float bias)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap,projCoords.xy,0).x;
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 0.5 : 0.0;

    return shadow;
}

//VICTOR GORDAN
float ShadowCalculation2(vec4 fragPosLight,float bias)
{
	// Shadow value
	float shadow = 0.0f;
	// Sets lightCoords to cull space
	vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;
	if(lightCoords.z <= 1.0f)
	{
		// Get from [-1, 1] range to [0, 1] range just like the shadow map
		lightCoords = (lightCoords + 1.0f) / 2.0f;
		float currentDepth = lightCoords.z;
		// Prevents shadow acne
		
		// Smoothens out the shadows
		int sampleRadius = 2;
		vec2 pixelSize = 1.0 / textureSize(shadowMap, 0);
		for(int y = -sampleRadius; y <= sampleRadius; y++)
		{
		    for(int x = -sampleRadius; x <= sampleRadius; x++)
		    {
		        float closestDepth = texture(shadowMap, lightCoords.xy + vec2(x, y) * pixelSize).r;
				if (currentDepth > closestDepth + bias)
					shadow += 1.0f;     
		    }    
		}
		shadow /= pow((sampleRadius * 2 + 1), 2);
	}
	return shadow;
}


void main()
{
	ivec2 texCoords = ivec2(gl_FragCoord.xy);
	vec3 positionWorld = texelFetch(gPosition,ivec2(texCoords),0).xyz;
    vec3 normal = texelFetch(gNormal,ivec2(texCoords),0).xyz;    
    //vec3 ambient = texelFetch(gAmbiant,ivec2(texCoords),0).xyz;	
    vec3 diffuse = texelFetch(gDiffuse,ivec2(texCoords),0).xyz;	
    vec3 specular = texelFetch(gSpecular,ivec2(texCoords),0).xyz;	
    float shininess = texelFetch(gSpecular,texCoords,0).w;
    float AmbientOcclusion = texelFetch(ssao,texCoords,0).x;

	vec3 position2 = texelFetch(gPosition2,ivec2(texCoords),0).xyz;

	 vec4 position = worldToViewMatrix * vec4(positionWorld,1);
    
     //vec3 ambientWithAO = vec3(AmbientOcclusion * ambient);
    vec3 ambientOccluded = vec3(0.3 * diffuse * AmbientOcclusion);
     
	vec3 viewDir = normalize( - position.xyz);
	vec3 lightDir = normalize(position.xyz - lightPos);

	vec3 H = normalize(viewDir - lightDir);

	float cosThetaPowShininess = 0;

	cosThetaPowShininess = pow(max(dot(normal,H),0.0f),shininess);

    vec3 specularLighting = specular.xxx * cosThetaPowShininess;
	float cosTheta = max(dot(normalize(normal),normalize(lightPos-position.xyz)),0.f);
	vec3 diffuseLight = diffuse * cosTheta;
	
	vec4 fragPosLight = lightSpaceMatrix * vec4(positionWorld, 1.0);
	float biasShadowMap = max(0.025f * (1.0f - dot(normal, lightDir)), 0.0005f);
    float shadow = ShadowCalculation(fragPosLight ,biasShadowMap); 
     
	vec3 result =  ambientOccluded + ((diffuseLight.xyz * (1-shadow)) + (specularLighting * (1-shadow)));
    
    // TONE MAPPING
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
    
    // LIGHT ATTENUATION
    float A = lightPower; 
    float r = lightAttenuationDistance;
    float pi = 3.14159265359;
    float constant = 1.0;
    float  linear = (4 * pi * r * r) / A;
     float quadratic = (4 * pi * r * r) / (A * A);
    float distanceLightObject = dot(normal,normalize(lightPos-position.xyz));
	float facteurAtenuation = 1.0 / (constant + linear * distanceLightObject + quadratic * distanceLightObject * distanceLightObject); 
	result *= facteurAtenuation;
	
	result = vec3(pow(result.x,1.5),pow(result.y,1.5),pow(result.z,1.5));

	result = ((result*(a*result+b))/(result*(c*result+d)+e));
    
    fragColor = vec4(result,1.0);

	
	//fragColor = vec4(diffuseLight * AmbientOcclusion  + specularLighting,1) ;
   
    //fragColor = vec4(vec3(1-shadow),1);
   // fragColor = vec4(position2,1);
    

}
