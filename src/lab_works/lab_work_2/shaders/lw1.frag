#version 450

layout( location = 0 ) out vec4 fragColor;
in vec4 aFragColor;

void main() {
	fragColor = aFragColor;
}
