#ifndef SHADER_NODES_LIGHT_GLSL
#define SHADER_NODES_LIGHT_GLSL

struct Light {
    highp vec3 color;
    highp float strength;
    highp vec3 position;
    highp float attenuation;
    highp float gamma;
};

highp float attenuation(highp Light light, highp vec3 vertexPosition, highp float attenuationOffset) {
    highp float distanceToLight = distance(vertexPosition, light.position) - attenuationOffset;
    highp float attenuationFactor = 1.0 / (1.0 + light.attenuation * 0.0001 * distanceToLight * distanceToLight);
    return attenuationFactor;
}

highp vec3 gamma(highp vec3 color, highp float gamma) {
    return pow(color, vec3(1.0/gamma));
}

#endif
