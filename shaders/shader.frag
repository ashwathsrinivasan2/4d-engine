#version 450


layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec4 worldNormal;
layout(location = 2) in vec4 texCoord;
layout(location = 3) in vec4 worldPos;
layout(location = 4) in vec4 camPos;
layout(location = 5) in vec4 scale;

layout(location = 0) out vec4 outColor;

vec4 lightPos = vec4(0.f, 0.f, 0.f, 0.f);
vec3 ambient = vec3(0.1f, 0.1f, 0.1f);

float attenuation(vec4 fragPos, vec4 lightPos, float radius) {
    float dist = length(lightPos - fragPos);
    float x = clamp(1.0 - pow(dist / radius, 4.0), 0.0, 1.0);
    return (x * x) / (dist * dist + 1.0);
}

vec3 calculateTexColor(vec3 texCoord){
    vec3 colorA = vec3(0.8);
    vec3 colorB  = vec3(0.3);

    float checker = mod(floor(worldPos.x) + floor(worldPos.y) + floor(worldPos.z), 2.0);
    vec3 color = mix(colorA, colorB, checker);
    return color;
}

vec3 hueToRGB(float h){
    vec3 rgb;
    float kr = mod(5.f+h*6.f, 6.f);
    float kg = mod(3.f+h*6.f, 6.f);
    float kb = mod(1.f+h*6.f, 6.f);

    rgb.r = 1.f - max(min(kr, min(4.f-kr, 1.f)), 0.f);
    rgb.g = 1.f - max(min(kg, min(4.f-kg, 1.f)), 0.f);
    rgb.b = 1.f - max(min(kb, min(4.f-kb, 1.f)), 0.f);

    return rgb;
}

vec3 calculateSierpenskiColor(vec3 texCoord){
    vec3 colorA = vec3(1.f);
    vec3 colorB  = vec3(0.3f);

    float size = 6.5f;

    texCoord *= scale.xyz;
    float x = mod(texCoord.x, size) / size;
    float y = mod(texCoord.y, size) / size;
    float z = mod(texCoord.z, size) / size;

    int detail = 5;
    bool inside = true;
    float level = 0.f;
    for(int i = 0; i < detail; i++){
        float currSize = 1.f / pow(3.f, float(i));
        float lowerBound = currSize / 3.f;
        float upperBound = lowerBound * 2.f;
        x = mod(x, currSize);
        y = mod(y, currSize);
        z = mod(z, currSize);

        bool check = inside && x > lowerBound && x < upperBound && y > lowerBound && y < upperBound && z > lowerBound && z < upperBound;
        inside = check ? false : inside;
        level = check ? float(i) / float(detail - 1) : level;
    
    }


    vec3 color = (1.f - level) * colorA + level * colorB;
    return color * fragColor.rgb;
}

void main(){
    lightPos = camPos;
    vec4 lightDirection = normalize(worldPos - lightPos);
    float distance = length(worldPos - lightPos);

    distance = max(distance, 0.5);
    float attenuation = attenuation(worldPos, lightPos, 30.f);
    float dotProduct = dot(lightDirection, -normalize(worldNormal));
    dotProduct = dotProduct < 0.f ? 0.f : dotProduct;
    distance = distance < 1.f ? 1.f : distance;
    float intensity = 10.f * dotProduct * attenuation;

    vec3 color = calculateSierpenskiColor(texCoord.rgb);
    outColor = vec4(color * (intensity + ambient), 1.f);
    clamp(outColor, 0.f, 1.f);

}

