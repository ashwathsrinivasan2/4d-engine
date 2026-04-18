#version 450

layout(binding = 0) uniform UniformBufferObject{
    mat4 view;
    mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 worldNormal;
layout(location = 2) out vec3 worldPos;

layout(push_constant) uniform Push{
    mat4 modelMat;
}push;

void main(){
    worldNormal = normalize((transpose(inverse(push.modelMat)) * vec4(inNormal, 1.f)).xyz);
    worldPos = (push.modelMat * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.proj * ubo.view * vec4(worldPos, 1.f);
    fragColor = inColor;
}