#version 330
#pragma shadernodes header

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec4 vertexTangent;
in vec2 vertexTexCoord;

out vec3 position;
out vec3 normal;
out vec3 tangent;
out vec3 binormal;
out vec2 texCoord;

uniform mat4 modelMatrix;
uniform mat3 modelNormalMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 mvp;

void main() {
    texCoord = vertexTexCoord;
    position = vec3(modelMatrix*vec4(vertexPosition, 1.0));

    tangent = normalize(modelNormalMatrix * vertexTangent.xyz);
    normal = normalize(modelNormalMatrix * vertexNormal.xyz);
    binormal = normalize(cross(normal, tangent));

#pragma shadernodes body

    // not mvp because position may have been displaced by shader nodes
    gl_Position = projectionMatrix*viewMatrix*vec4(position, 1.0);
}
