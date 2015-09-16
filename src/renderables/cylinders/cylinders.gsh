layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;
in vec3 vs_vertex1Position[1];
in vec3 vs_vertex2Position[1];

out vec2 texCoord;
out vec3 vertexPosition;
out float da;
out float mode;

void main(void) {
    float radius = 0.5;
    vec3 v1 = vs_vertex1Position[0];
    vec3 v2 = vs_vertex2Position[0];
    vec3 delta = v2 - v1;
    // Project delta onto screen (remove anything orthogonal on screen)
    vec3 deltaProjected = delta - dot(delta, cp_viewVector)*cp_viewVector;
    vec3 deltaProjectedNormalized = normalize(deltaProjected);
    vec3 orthogonalOnDelta = cross(deltaProjectedNormalized, cp_viewVector);
    float costheta = normalize(dot(deltaProjectedNormalized, delta));
    da = radius*costheta;

    vec3 cameraToV1 = cp_cameraPosition - v1;
    vec3 cameraToV2 = cp_cameraPosition - v2;
    float distanceToV1 = dot(cameraToV1, cameraToV1);
    float distanceToV2 = dot(cameraToV2, cameraToV2);
    if(distanceToV1 < distanceToV2) {
        mode = 0.0;
        // v1 closer than v2
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 + orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(0, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 + orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(1.0, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 - orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(0, -1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 - orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(1.0, -1.0);
        EmitVertex();
        EndPrimitive();
    } else {
        mode = 1.0;
        // v2 closer than v1
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 + orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(0, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 + orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(1.0, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 - orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(0, -1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 - orthogonalOnDelta*radius, 1.0);
        texCoord = vec2(1.0, -1.0);
        EmitVertex();
        EndPrimitive();
    }
}
