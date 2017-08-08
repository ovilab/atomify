#ifndef SHADER_NODES_STANDARD_MATERIAL_GLSL
#define SHADER_NODES_STANDARD_MATERIAL_GLSL

highp vec3 standardMaterialLight(highp Light light, highp vec3 vnormal, highp vec3 vertexPosition, highp vec3 cameraPosition,
                        highp vec3 ambientColor, highp vec3 diffuseColor, highp vec3 specularColor,
                        highp float ambientIntensity, highp float diffuseIntensity, highp float specularIntensity,
                        highp float hardness, highp float attenuationOffset) {
    highp vec3 normal = normalize(vnormal); // IMPORTANT: Do not remove
    // this because the normal is no longer normalized due to
    // interpolation on fragment shader

    highp vec3 lightVector = vec3(0.0, 0.0, 0.0);
    highp vec3 lightPosition = light.position;

    highp vec3 surfaceToLight = normalize(lightPosition - vertexPosition);
    highp float attenuationFactor = attenuation(light, vertexPosition, attenuationOffset);

    highp vec3 surfaceToCamera = normalize(cameraPosition - vertexPosition);


    /* AMBIENT */
    lightVector += ambientColor.rgb*ambientIntensity;

    /* DIFFUSE */
    highp float diffuseCoefficient1 = max(0.0, dot(normal, surfaceToLight));
    lightVector += light.color*light.strength*diffuseColor.rgb*diffuseCoefficient1*diffuseIntensity*attenuationFactor;

    /* SPECULAR */
    highp vec3  reflectionVector = reflect(-surfaceToLight, normal);
    highp float cosAngle = dot(reflectionVector, surfaceToCamera);
    highp float normFactor = (hardness + 2.0) / 2.0;
    // normFactor *= (cosAngle < 3.1415) ? 1.0 : 0.0;
    highp float specularCoefficient = pow(max(cosAngle, 0.0), hardness);
    lightVector += normFactor*light.color*light.strength*specularColor.rgb*specularCoefficient*specularIntensity*attenuationFactor;

   /* RETURN GAMMA CORRECTED COMBINED */
   return gamma(lightVector, light.gamma);
}

#endif
