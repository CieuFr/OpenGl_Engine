#version 450
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;

out vec2 TexCoords;
out vec4 fragPosLight;



void main()
{
    TexCoords = texCoords;
    
    gl_Position = vec4(position, 1.0);

}
