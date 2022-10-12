#version 450

layout( location = 0 ) in vec4 aVertexPosition;
layout( location = 1 ) in vec4 aVertexColor;
out vec4 aFragColor;
uniform mat4 matrixLtoW;
uniform mat4 matrixWtoV;
uniform mat4 matrixVtoC;


void main() {
	mat4 matrixLtoC = matrixVtoC * matrixWtoV * matrixLtoW;
	gl_Position = matrixLtoC * aVertexPosition ;
	aFragColor = aVertexColor;

}
