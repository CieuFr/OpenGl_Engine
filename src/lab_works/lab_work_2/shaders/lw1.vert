#version 450

layout( location = 0 ) in vec4 aVertexPosition;
layout( location = 1 ) in vec4 aVertexColor;
out vec4 aFragColor;
uniform float uTranslationX;


void main() {
	gl_Position = aVertexPosition;
	gl_Position.x += uTranslationX;
	aFragColor = aVertexColor;

}
