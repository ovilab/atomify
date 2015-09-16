layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;
in vec3 vs_vertex1Position[1];
in vec3 vs_vertex2Position[1];

out vec2 texCoord;
out vec3 vertexPosition;
out float da;

void main(void) {
    float radius = 0.2;
    float sphereRadius = 0.96; // Spheres discard any r^2 > 0.9 => r>0.95
    float dr = sqrt(sphereRadius*sphereRadius - radius*radius);
    vec3 epsilonAwayFromCamera = cp_viewVector*0.001;

    // Move vertices slightly away from camera so they always appear behind spheres
    vec3 v1 = vs_vertex1Position[0]+epsilonAwayFromCamera;
    vec3 v2 = vs_vertex2Position[0]+epsilonAwayFromCamera;
    vec3 delta = v2 - v1;
    vec3 deltaNormalized = normalize(delta);

    // Project delta onto screen (remove anything orthogonal on screen)
    vec3 deltaProjected = delta - dot(delta, cp_viewVector)*cp_viewVector;
    vec3 deltaProjectedNormalized = normalize(deltaProjected);
    vec3 orthogonalOnDelta = cross(deltaProjectedNormalized, cp_viewVector);
    vec3 orthogonalOnDeltaTimesRadius = orthogonalOnDelta*radius;

    float costheta = 0.5*abs(dot(cp_viewVector, normalize(delta))); // Cheating or being stupid
    // float costheta = 1.0 - dot(deltaProjectedNormalized, normalize(delta));
    da = radius*costheta;

    vec3 cameraToV1 = cp_cameraPosition - v1;
    vec3 cameraToV2 = cp_cameraPosition - v2;
    float distanceToV1 = dot(cameraToV1, cameraToV1);
    float distanceToV2 = dot(cameraToV2, cameraToV2);

    if(distanceToV1 < distanceToV2) {
        // v1 closer than v2
        v2 = v2 - deltaNormalized*dr;
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 - orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(-da, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 - orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(1.0, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 + orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(-da, -1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 + orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(1.0, -1.0);
        EmitVertex();
        EndPrimitive();
    } else {
        // v2 closer than v1
        v1 = v1 + deltaNormalized*dr;
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 + orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(-da, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 + orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(1.0, 1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v1 - orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(-da, -1.0);
        EmitVertex();
        gl_Position = cp_modelViewProjectionMatrix*vec4(v2 - orthogonalOnDeltaTimesRadius, 1.0);
        texCoord = vec2(1.0, -1.0);
        EmitVertex();
        EndPrimitive();
    }
}
