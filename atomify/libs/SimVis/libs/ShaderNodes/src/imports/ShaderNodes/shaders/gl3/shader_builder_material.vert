#version 330
#pragma shadernodes header

in highp vec3 vertexPosition;
in highp vec3 vertexNormal;
in highp vec4 vertexTangent;
in highp vec2 vertexTexCoord;

out highp vec3 position;
out highp vec3 normal;
out highp vec3 tangent;
out highp vec3 binormal;
out highp vec2 texCoord;

uniform highp mat4 modelMatrix;
uniform highp mat3 modelNormalMatrix;
uniform highp mat4 viewMatrix;
uniform highp mat4 projectionMatrix;
uniform highp mat4 mvp;

highp vec3 makePerpendicular(highp vec3 v) {
    if(v.x == 0.0 && v.y == 0.0) {
        if(v.z == 0.0) {
            return vec3(0.0, 0.0, 0.0);
        }
        return vec3(0.0, 1.0, 0.0);
    }
    return vec3(-v.y, v.x, 0.0);
}

void main() {
    texCoord = vertexTexCoord;
    position = vec3(modelMatrix*vec4(vertexPosition, 1.0));

    // TODO something is wrong with tangent in GLSL 3.3
    normal = vertexNormal;
    if(length(normal) <= 0.0) {
        normal = vec3(0.0, 0.0, 1.0);
    }
    normal = normalize(modelNormalMatrix * normal);
    tangent = vertexTangent.xyz;
    if(length(tangent) <= 0.0) {
        tangent = makePerpendicular(normal);
    }
    tangent = normalize(modelNormalMatrix * tangent.xyz);
    binormal = normalize(cross(normal, tangent));

#pragma shadernodes body

    // not mvp because position may have been displaced by shader nodes
    gl_Position = projectionMatrix*viewMatrix*vec4(position, 1.0);
}
