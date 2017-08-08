#version 330
#pragma shadernodes header

in highp vec3 position;
in highp vec2 texCoord;
in highp vec3 normal;
in highp vec3 tangent;
in highp vec3 binormal;

out highp vec4 fragColor;

uniform highp vec3 eyePosition;
uniform highp mat3 modelNormalMatrix;
uniform highp mat4 modelViewMatrix;
uniform highp mat4 modelMatrix;
uniform highp mat4 modelViewProjectionMatrix;

void main(void) {
#pragma shadernodes body
}

