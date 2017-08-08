#version 330

layout( points ) in;
layout( triangle_strip, max_vertices = 12 ) out;
//uniform float radius;
in vec3 vs_vertex1Position[1];
in vec3 vs_vertex2Position[1];
in float vs_radius1[1];
in float vs_radius2[1];

out vec2 texCoord;
out vec3 modelViewPosition;
out vec3 cylinderDirection;
out mat3 cylinderBasis;
out mat3 cylinderWorldBasis;
out float da;
out float radius;
out float radiusA;
out float radiusB;
out vec3 base;
out vec3 end;
out vec3 color;
out vec3 v1;
out vec3 v2;
out vec3 worldPosition;

uniform mat4 mvp;
uniform mat4 modelViewMatrix;
uniform mat3 modelViewNormalMatrix;
uniform mat4 projectionMatrix;
uniform vec3 eyePosition;

void main(void) {
    gl_Position = mvp*vec4(0.0, 0.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = mvp*vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = mvp*vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();

    gl_Position = mvp*vec4(1.0, 1.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();

    mat4 MV = modelViewMatrix;
    mat4 MVP = mvp;
    mat4 P = projectionMatrix;

    v1 = vs_vertex1Position[0];
    v2 = vs_vertex2Position[0];

    vec3 delta = v2 - v1;
    vec3 deltaNormalized = normalize(delta);

    radiusA = vs_radius1[0];
    radiusB = vs_radius2[0];

    float rmax = max(radiusA, radiusB);

    vec3 center = (v1 + v2) * 0.5;
    vec3 cam_dir = (eyePosition - center);

    vec3 right = cross(cam_dir, deltaNormalized);
    right = normalize(right);

    vec3 outward = cross(deltaNormalized, right);
    outward = normalize(outward);

    // multiplying with normal matrix is the same as multiplying
    // with the upper 3x3 part of the modelview matrix,
    // except that it doesn't change vector lengths
    // and preserves normals when the modelview matrix has
    // non-uniform scaling
    cylinderBasis = mat3(modelViewNormalMatrix * outward, // U
                         modelViewNormalMatrix * right, // V
                         modelViewNormalMatrix * deltaNormalized); // axis

    cylinderWorldBasis = mat3(normalize(outward), // U
                         normalize(right), // V
                         normalize(deltaNormalized)); // axis

    vec4 mvv1 = MV * vec4(v1, 1.0);
    vec4 mvv2 = MV * vec4(v2, 1.0);
    // NOTE: dividing by w here is typically not necessary
    // w is seldom modified by model view matrices
    base = mvv1.xyz / mvv1.w;
    end = mvv2.xyz / mvv2.w;

    vec3 cameraToV1 = eyePosition - v1;
    vec3 cameraToV2 = eyePosition - v2;
    float distanceToV1 = dot(cameraToV1, cameraToV1);
    float distanceToV2 = dot(cameraToV2, cameraToV2);

    if(distanceToV1 > distanceToV2) {
        vec3 vtmp = v1;
        v1 = v2;
        v2 = vtmp;
    }

    vec3 vertices[6];
    vertices[ 0] = v1 - right*rmax - outward*rmax; // cap top left
    vertices[ 1] = v1 + right*rmax - outward*rmax;
    vertices[ 2] = v1 - right*rmax + outward*rmax; // cap bottom left
    vertices[ 3] = v1 + right*rmax + outward*rmax;

    vertices[ 4] = v2 - right*rmax + outward*rmax; // main bottom left
    vertices[ 5] = v2 + right*rmax + outward*rmax;

    vec2 texCoords[6];
    texCoords[ 0] = vec2(-1.0, -1.0);
    texCoords[ 1] = vec2(1.0, -1.0);
    texCoords[ 2] = vec2(-1.0, 1.0);
    texCoords[ 3] = vec2(1.0, 1.0);

    texCoords[ 4] = vec2(-1.0, 0.0);
    texCoords[ 5] = vec2(1.0, 0.0);

    for(int i = 0; i < 6; i++) {
        color = vec3(1.0, 1.0, 1.0);
        modelViewPosition = (MV*vec4(vertices[i], 1.0)).xyz;
        worldPosition = vertices[i];
        gl_Position = P*MV*vec4(vertices[i], 1.0);
        texCoord = texCoords[i];
        EmitVertex();
    }

    EndPrimitive();

}





