#version 450

layout( location = 0 ) in vec3 aVertexPosition;


uniform mat4 lightSpaceMatrix;
uniform mat4 model;


void main()
{
    gl_Position = lightSpaceMatrix * model* vec4(aVertexPosition, 1.0);
}
