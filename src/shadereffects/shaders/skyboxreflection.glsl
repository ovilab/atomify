// BEGIN skyboxreflection.glsl
#ifndef SKYBOXREFLECTIONGLSL
#define SKYBOXREFLECTIONGLSL
uniform samplerCube skyMapTexture;
uniform highp float cp_skyboxReflectivity;

highp vec3 skyboxReflection(highp vec3 normal, highp vec3 vertexPosition) {
    highp vec3 surfaceToCamera = normalize(cp_cameraPosition - vertexPosition);
    highp vec3  reflectionVector = reflect(-surfaceToCamera, normal);
    highp vec4 color = textureCube(skyMapTexture, reflectionVector);
    return color.xyz*cp_skyboxReflectivity;
}

#endif
// END skyboxreflection.glsl
