#version 450

layout( location = 0 ) out vec4 fragColor;

in vec3 normal;
in vec4 position;

uniform vec3 ambient;
uniform float luminosity;
uniform vec3 diffuse;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform vec3 specular;
uniform float shininess;



void main()
{
	vec3 viewDir = normalize(cameraPos - position.xyz);
	vec3 lightDir = normalize(position.xyz - lightPos);

	float normalDirection = dot(normal,viewDir);

	vec3 normalAfterCheck;

	if(normalDirection < 0) {
		normalAfterCheck = -normal;
	} else {
		normalAfterCheck = normal;
	}

	


	vec3 H = normalize(viewDir - lightDir);

	//vec3 reflectDir = reflect(lightDir,normal);
	vec3 spec =  pow(max(dot(normalAfterCheck,H),0.0),shininess) * specular;

	float diff = max(dot(normalAfterCheck,normalize(lightPos-position.xyz)),0.f);
	vec3 diffuseLight = diffuse * diff ;


	vec3 result = (ambient + diffuseLight + spec);


	fragColor =  vec4(result,1.0)  ;
}
