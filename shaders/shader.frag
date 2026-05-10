#version 450


layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 worldNormal;
layout(location = 2) in vec4 texCoord;
layout(location = 3) in vec4 worldPos;

layout(location = 0) out vec4 outColor;

vec4 lightPos = vec4(0.f, 0.f, 0.f, 0.f);
vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

float attenuation(vec4 fragPos, vec4 lightPos, float radius) {
    float dist = length(lightPos - fragPos);
    float x = clamp(1.0 - pow(dist / radius, 4.0), 0.0, 1.0);
    return (x * x) / (dist * dist + 1.0);
}

void main(){
    vec4 lightDirection = normalize(worldPos - lightPos);
    float distance = length(worldPos - lightPos);
    distance = max(distance, 0.5);
    float attenuation = attenuation(worldPos, lightPos, 30.f);
    float dotProduct = dot(lightDirection, -normalize(worldNormal));
    dotProduct = dotProduct < 0.f ? 0.f : dotProduct;
    distance = distance < 1.f ? 1.f : distance;
    float intensity = 10.f * dotProduct * attenuation;

    vec3 color = texCoord.rgb;
    outColor = vec4(color * (intensity + ambient), 1.f);
    clamp(outColor, 0.f, 1.f);
 
    //outColor = fragColor;

}

