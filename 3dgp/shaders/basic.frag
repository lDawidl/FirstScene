// FRAGMENT SHADER

#version 330

uniform bool tex; // this will be used to freely turn textures on and off on objects in main

in vec4 position;

in vec3 normal;

in vec4 color;

out vec4 outColor;

uniform sampler2D texture0;

in vec2 texCoord0;

// Materials

uniform vec3 materialAmbient;

uniform vec3 materialDiffuse;

uniform vec3 materialSpecular;

uniform float shininess;


// View Matrix

uniform mat4 matrixView;

struct POINT

{

vec3 position;

vec3 diffuse;

vec3 specular;

};

uniform POINT lightPoint1, lightPoint2;

vec4 PointLight(POINT light)
{
    vec4 color = vec4(0,0,0,0);

    // Light position in view space
    vec3 lightPosView = vec3(matrixView * vec4(light.position, 1.0));

    // Vector 
    vec3 L = normalize(lightPosView - position.xyz);

    // Diffuse lighting
    float NdotL = dot(normal, L);
    color +=  vec4(materialDiffuse * light.diffuse, 1.0) * max(NdotL, 0);

    vec3 V = normalize(-position.xyz);
    vec3 R = reflect(-L, normal);
    float RdotV = dot(R, V);
    color += vec4(materialSpecular * light.specular * pow(max(RdotV, 0), shininess), 1);

    return color;
}



void main(void)

{


outColor = color;
outColor += PointLight(lightPoint1);
outColor += PointLight(lightPoint2);
if (tex)
    outColor *= texture(texture0, texCoord0);
}
