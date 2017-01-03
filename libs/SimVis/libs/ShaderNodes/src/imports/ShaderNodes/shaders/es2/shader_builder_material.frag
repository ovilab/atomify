#version 330
#pragma shadernodes header

in vec3 position;
in vec2 texCoord;
in vec3 normal;
in vec3 tangent;
in vec3 binormal;

out vec4 fragColor;

uniform vec3 eyePosition;
uniform mat3 modelNormalMatrix;
uniform mat4 modelViewMatrix;
uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;

void main(void) {
#pragma shadernodes body
}
