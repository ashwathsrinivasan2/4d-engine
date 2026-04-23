#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 worldNormal;
layout(location = 2) in vec3 worldPos;

layout(location = 0) out vec4 outColor;

layout(push_constant) uniform Push{
    mat4 modelMat;
}push;

vec3 lightPos = vec3(5.f, 10.f, 10.f);
vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

void main(){
    vec3 lightDirection = normalize(worldPos - lightPos);
    float distance = length(worldPos - lightPos);
    float dotProduct = dot(lightDirection, -worldNormal);
    dotProduct = dotProduct < 0.f ? 0.f : dotProduct;
    distance = distance < 1.f ? 1.f : distance;
    float intensity = 10.f * dotProduct / (distance * distance);

    outColor = vec4(fragColor * intensity + fragColor * ambient, 1.f);
    clamp(outColor, 0.f, 1.f);

}