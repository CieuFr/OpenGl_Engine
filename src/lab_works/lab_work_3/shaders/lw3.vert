#version 450

layout( location = 0 ) in vec4 aVertexPosition;
layout( location = 1 ) in vec4 aVertexColor;
out vec4 aFragColor;

uniform mat4 transformationMatrix;


void main() {

	gl_Position = transformationMatrix * aVertexPosition ;
	aFragColor = aVertexColor;

}
