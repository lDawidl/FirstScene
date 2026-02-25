#version 330


in float age;

uniform sampler2D texture0;

out vec4 outColor;


void main()

{

outColor = texture(texture0, gl_PointCoord);


// colour calculation – good for PNG images with alpha channel

outColor.a *= 1 - age;

float gradient = pow(1 - age, 1.5);

vec3 yellow = vec3(1, 1, 0);

vec3 red = vec3(1, 0, 0);

outColor.rgb = mix(yellow, red, 1 - gradient);

}