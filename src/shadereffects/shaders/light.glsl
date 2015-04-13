// BEGIN light.glsl
#ifndef LIGHTGLSL
#define LIGHTGLSL
// Light based on http://www.tomdalling.com/blog/modern-opengl/07-more-lighting-ambient-specular-attenuation-gamma/
// and http://www.tomdalling.com/blog/modern-opengl/08-even-more-lighting-directional-lights-spotlights-multiple-lights/

const int maxLights = 8;
uniform int numLights;
struct Light {
    highp vec4 ambientColor;
    highp vec4 diffuseColor;
    highp vec4 specularColor;
    highp vec3 position;
    highp float attenuation;
    highp float shininess;
    highp float gamma;
    highp float diffuseIntensity;
    highp float ambientIntensity;
    highp float specularIntensity;
};

uniform Light cp_lights[maxLights];
uniform highp int cp_numberOfLights;

highp float attenuation(Light light, highp vec3 vertexPosition) {
    highp float distanceToLight = distance(vertexPosition, light.position);
    highp float attenuationFactor = 1.0 / (1.0 + light.attenuation * distanceToLight * distanceToLight);
    return attenuationFactor;
}

highp vec3 gamma(highp vec3 color, highp float gamma) {
    return pow(color, vec3(1.0/gamma));
}

highp vec3 applyLight(Light light, highp vec3 normal, highp vec3 vertexPosition, highp vec3 color) {
    highp vec3 lightVector = vec3(0.0);

#if defined(DEFAULTLIGHTSPECULAR) || defined(DEFAULTLIGHTDIFFUSE)
    highp vec3 surfaceToLight = normalize(light.position - vertexPosition);
    highp float attenuationFactor = attenuation(light, vertexPosition);
#endif

#if defined(DEFAULTLIGHTSPECULAR) || defined(DEFAULTLIGHTAMBIENT)
    highp vec3 surfaceToCamera = normalize(cp_cameraPosition - vertexPosition);
#endif

   /* DIFFUSE */
#ifdef DEFAULTLIGHTDIFFUSE
    highp float diffuseCoefficient1 = max(0.0, dot(normal, surfaceToLight));
    // smooths the edges of the light (for some reason)
    diffuseCoefficient1 *= diffuseCoefficient1;
    lightVector += color*light.diffuseColor.rgb*diffuseCoefficient1*light.diffuseIntensity*attenuationFactor;
#endif

   /* AMBIENTDIFFUSE */
#ifdef DEFAULTLIGHTAMBIENT
    highp float diffuseCoefficient2 = max(0.0, dot(normal, surfaceToCamera));
    lightVector += color*light.ambientColor.rgb*light.ambientIntensity*(vec3(diffuseCoefficient2)*0.9 + 0.1*vec3(1.0));
#endif

    /* SPECULAR */
#ifdef DEFAULTLIGHTSPECULAR
    highp vec3  reflectionVector = reflect(-surfaceToLight, normal);
    highp float cosAngle = max(0.0, dot(surfaceToCamera, reflectionVector));
    highp float specularCoefficient = pow(cosAngle, light.shininess);
    lightVector += light.specularColor.rgb*specularCoefficient*light.specularIntensity*attenuationFactor;
#endif

   /* RETURN GAMMA CORRECTED COMBINED */
   return gamma(lightVector, light.gamma);
}

highp vec3 defaultLight(highp vec3 normal, highp vec3 vertexPosition, highp vec3 color) {
    highp vec3 light = vec3(0.0);
    highp float oneOverNumberOfLights = 1.0/max(float(cp_numberOfLights), 1.0);
    for(int i=0; i<cp_numberOfLights; i++) {
        light += oneOverNumberOfLights*applyLight(cp_lights[i], normal, vertexPosition, color);
    }

    return light;
}

highp vec3 defaultLight(highp vec3 normal, highp vec3 vertexPosition, highp vec4 color) {
    return defaultLight(normal, vertexPosition, color.rgb);
}
#endif
// END light.glsl
