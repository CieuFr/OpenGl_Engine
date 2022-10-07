#version 450

layout( location = 0 ) out vec4 fragColor;
in vec4 aFragColor;
uniform float luminosity;


void main() {
	fragColor = aFragColor * luminosity;
}
