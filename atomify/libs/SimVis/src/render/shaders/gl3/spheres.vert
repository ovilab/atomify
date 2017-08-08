#version 330

in vec3 vertexPosition;
in float vertexId;
in vec3 pos;
in vec3 col;
in float scale;

uniform vec3 eyePosition = vec3(0.0, 0.0, 0.0);

uniform mat4 modelMatrix;
uniform mat4 mvp;

out vec3 modelSpherePosition;
out vec3 modelPosition;
out vec3 color;
out vec2 planePosition;
out float radius;
vec3 makePerpendicular(vec3 v) {
    if(v.x == 0.0 && v.y == 0.0) {
        if(v.z == 0.0) {
            return vec3(0.0, 0.0, 0.0);
        }
        return vec3(0.0, 1.0, 0.0);
    }
    return vec3(-v.y, v.x, 0.0);
}

void main() {
    vec3 position = vertexPosition + pos;
    color = col;
    radius = scale;
    modelSpherePosition = (modelMatrix * vec4(position, 1.0)).xyz;

    vec3 view = normalize(position - eyePosition);
    vec3 right = normalize(makePerpendicular(view));
    vec3 up = cross(right, view);

    float texCoordX = 1.0 - 2.0*(float(vertexId==0.0) + float(vertexId==2.0));
    float texCoordY = 1.0 - 2.0*(float(vertexId==0.0) + float(vertexId==1.0));
    planePosition = vec2(texCoordX, texCoordY);

    // TODO should find needed size or move closer to camera. NOTE: 0.6 is a hack, should be 0.5
    // Another factor of 2 since the scale is somehow the diameter?
    position += 2*0.6*(-up - right)*(scale*float(vertexId==0.0));
    position += 2*0.6*(-up + right)*(scale*float(vertexId==1.0));
    position += 2*0.6*(up - right)*(scale*float(vertexId==2.0));
    position += 2*0.6*(up + right)*(scale*float(vertexId==3.0));

    vec4 modelPositionTmp = modelMatrix * vec4(position, 1.0);
    modelPosition = modelPositionTmp.xyz;

    gl_Position = mvp*vec4(position, 1.0);
}
