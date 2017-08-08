#version 330

in vec3 vertex1Position;
in vec3 vertex2Position;
in float radius1;
in float radius2;
in float sphereRadius1;
in float sphereRadius2;
in float vertexId;

out vec3 vs_vertex1Position;
out vec3 vs_vertex2Position;
out float vs_sphereRadius1;
out float vs_sphereRadius2;
out float vs_radius1;
out float vs_radius2;

out vec2 texCoord;
out vec3 modelViewPosition;
out vec3 modelPosition;
out vec3 worldPosition;
out vec3 base;
out vec3 end;
out vec3 color;
out float radiusA;
out float radiusB;
out float da;
out mat3 cylinderBasis;
out vec3 cylinderDirection;
out mat3 cylinderWorldBasis;

out vec3 perpendicular;
out vec3 biperpendicular;

uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 projectionMatrix;
uniform vec3 eyePosition;

vec3 makePerpendicular(vec3 v) {
    if(v.x == 0.0 && v.y == 0.0) {
        if(v.z == 0.0) {
            return vec3(0.0, 0.0, 0.0);
        }
        return vec3(0.0, 1.0, 0.0);
    }
    return vec3(-v.y, v.x, 0.0);
}

void main(void)
{
    vs_vertex1Position = vertex1Position;
    vs_vertex2Position = vertex2Position;
    vs_radius1 = radius1;
    vs_radius2 = radius2;
    vs_sphereRadius1 = sphereRadius1;
    vs_sphereRadius2 = sphereRadius2;

    vec3 v1 = vs_vertex1Position;
    vec3 v2 = vs_vertex2Position;

    vec3 mv1 = (modelMatrix * vec4(v1, 1.0)).xyz;
    vec3 mv2 = (modelMatrix * vec4(v2, 1.0)).xyz;

    vec3 delta = v2 - v1;
    vec3 deltaNormalized = normalize(delta);

    // define a unique perpendicular to the axis that can be used by
    // the texture coordinate in the fragment shader
    perpendicular = normalize(makePerpendicular(deltaNormalized));
    biperpendicular = normalize(cross(delta, perpendicular));

    radiusA = vs_radius1;
    radiusB = vs_radius2;

    float rmax = max(radiusA, radiusB);

    vec3 center = (mv1 + mv2) * 0.5;
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
    cylinderBasis = mat3(modelViewNormal * outward, // U
                         modelViewNormal * right, // V
                         modelViewNormal * deltaNormalized); // axis

    cylinderWorldBasis = mat3(normalize(outward), // U
                              normalize(right), // V
                              normalize(deltaNormalized)); // axis

    vec4 mvv1 = modelView * vec4(v1, 1.0);
    vec4 mvv2 = modelView * vec4(v2, 1.0);
    // NOTE: dividing by w here is typically not necessary
    // w is seldom modified by model view matrices
    base = mvv1.xyz / mvv1.w;
    end = mvv2.xyz / mvv2.w;

    vec3 cameraToV1 = eyePosition - mv1;
    vec3 cameraToV2 = eyePosition - mv2;
    float distanceToV1 = dot(cameraToV1, cameraToV1);
    float distanceToV2 = dot(cameraToV2, cameraToV2);

    float sign = 1.0;
    if(distanceToV1 > distanceToV2) {
        vec3 vtmp = v1;
        v1 = v2;
        v2 = vtmp;
        sign = -1.0; // keeps everything front face (in case of back face culling)
    }

    vec3 vertices[6];
    vertices[ 0] = v1 + sign*right*rmax - outward*rmax; // cap top left
    vertices[ 1] = v1 - sign*right*rmax - outward*rmax;
    vertices[ 2] = v1 + sign*right*rmax + outward*rmax; // cap bottom left
    vertices[ 3] = v1 - sign*right*rmax + outward*rmax;

    vertices[ 4] = v2 + sign*right*rmax + outward*rmax; // main bottom left
    vertices[ 5] = v2 - sign*right*rmax + outward*rmax;

    vec2 texCoords[6];
    texCoords[ 0] = vec2(-1.0, -1.0);
    texCoords[ 1] = vec2(1.0, -1.0);
    texCoords[ 2] = vec2(-1.0, 1.0);
    texCoords[ 3] = vec2(1.0, 1.0);

    texCoords[ 4] = vec2(-1.0, 0.0);
    texCoords[ 5] = vec2(1.0, 0.0);

    int i = int(vertexId);
    color = vec3(1.0, 1.0, 1.0);
    vec4 ppos = vec4(vertices[i], 1.0);
    modelPosition = (modelMatrix * ppos).xyz;
    modelViewPosition = (modelView * ppos).xyz;
    worldPosition = vertices[i];
    gl_Position = mvp*vec4(vertices[i], 1.0);
}
