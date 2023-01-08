#version 450

layout(location = 0) out float fragColor;

layout (binding = 0) uniform sampler2D gPosition;
layout (binding = 1) uniform sampler2D gNormal;
layout (binding = 2) uniform sampler2D texNoise;
layout (binding = 3) uniform sampler2D gPositionViewSpace;


uniform vec3 samples[64];
uniform int kernelSize;
uniform float radius;
uniform float bias;
uniform int power;



uniform mat4 projection;

void main()
{
   // vec2 noiseScale = vec2(1280/4.0, 720/4.0); 

	ivec2 texCoords = ivec2(gl_FragCoord.xy);

   
	vec3 position = texelFetch(gPositionViewSpace,ivec2(texCoords),0).xyz;

    
    vec3 normal = texelFetch(gNormal,ivec2(texCoords),0).xyz;    

//    vec3 texxyz = texture(texNoise, texCoords * noiseScale).xyz;
//    vec3 randomVec = normalize(texture(texNoise, texCoords * noiseScale).xyz);
    vec3 randomVec = normalize(texture(texNoise, texCoords ).xyz);
    
  
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
    
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
       
        vec3 samplePos = TBN * samples[i]; 
        samplePos = position + samplePos * radius; 
        
       
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPositionViewSpace, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(position.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
    fragColor = pow(occlusion, power);

  // fragColor =1.;
   
  // fragColor = vec3(position.x,position.y,position.z);
  // fragColor = vec3(normal.x,normal.y,normal.z);


 //  fragColor = vec3(0,1,1);


}
