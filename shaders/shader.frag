#version 450
layout(binding = 1) uniform sampler2D texSampler;

layout(binding = 2) uniform Light{
    vec3 pos;
    vec4 color;
}light;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 3) in vec3 fragPos;

layout(location = 0) out vec4 outColor;

void main() {
    vec3 normal = normalize(fragNormal);
    vec3 lightDir = normalize(light.pos - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * light.color.rgb * light.color.a;

    vec3 texColor = texture(texSampler, fragTexCoord).rgb;

    vec3 result = (diffuse + 0.1) * texColor;
    //outColor = vec4(normal * 0.5 + 0.5, 1.0);
    //outColor = vec4(lightDir * 0.5 + 0.5, 1.0);
    //outColor = vec4(normalize(light.pos - fragPos) * 0.5 + 0.5, 1.0);

    //outColor = vec4(light.color, 1.0);
    outColor = vec4(result, 1.0);
}