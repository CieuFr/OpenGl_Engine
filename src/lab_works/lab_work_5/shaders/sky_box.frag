#version 450

layout( location = 0 ) out vec4 fragColor;
layout (binding = 0) uniform samplerCube skybox;
in vec3 TexCoords;

void main() {
	 fragColor = texture(skybox, TexCoords);
	// fragColor = vec4(0.0, 1.0, 1.0, 1.0);
}
