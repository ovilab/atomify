// BEGIN simplexbump.glsl
#ifndef SIMPLEXBUMPGLSL
#define SIMPLEXBUMPGLSL
uniform highp float cp_bumpIntensity;
uniform highp float cp_bumpScale;

highp vec3 simplexbump(highp vec3 normal, highp vec3 vertexPosition) {
    highp float nx = snoise(0.5*(vertexPosition.xy + vertexPosition.zz + vec2(2.0,2.0))*cp_bumpScale);
    highp float ny = snoise(0.5*(vertexPosition.yz + vertexPosition.xx + vec2(-2.0,2.0))*cp_bumpScale);
    highp float nz = snoise(0.5*(vertexPosition.xz + vertexPosition.yy + vec2(2.0,-2.0))*cp_bumpScale);
    return normal + vec3(nx, ny, nz)*cp_bumpIntensity;
}

#endif
// END simplexbump.glsl
