layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;
uniform float radius;
in vec3 vs_vertex1Position[1];
in vec3 vs_vertex2Position[1];
in float vs_radius1[1];
in float vs_radius2[1];

out vec2 texCoord;
out vec3 vertexPosition;
out vec3 yDir;
out float da;

void main(void) {
    float sphereRadius1 = vs_radius1[0]*0.95; // Spheres discard any r^2 > 0.9 => r>0.95
    float sphereRadius2 = vs_radius2[0]*0.95; // Spheres discard any r^2 > 0.9 => r>0.95
    float dr1 = sqrt(sphereRadius1*sphereRadius1 - radius*radius);
    float dr2 = sqrt(sphereRadius2*sphereRadius2 - radius*radius);
    vec3 epsilonAwayFromCamera = cp_viewVector*0.001;

    // Move vertices slightly away from camera so they always appear behind spheres
    vec3 v1 = vs_vertex1Position[0]+epsilonAwayFromCamera;
    vec3 v2 = vs_vertex2Position[0]+epsilonAwayFromCamera;
    vec3 delta = v2 - v1;
    vec3 deltaNormalized = normalize(delta);

    // Project delta onto screen (remove anything orthogonal on screen)
    vec3 deltaProjected = delta - dot(delta, cp_viewVector)*cp_viewVector;
    vec3 deltaProjectedNormalized = normalize(deltaProjected);
    vec3 orthogonalOnDeltaNormalized = cross(deltaProjectedNormalized, cp_viewVector);
    vec3 orthogonalOnDeltaNormalizedTimesRadius = orthogonalOnDeltaNormalized*radius;

    float costheta = 0.5*abs(dot(cp_viewVector, deltaNormalized)); // Cheating or being stupid with the factor 0.5
    da = radius*costheta;

    vec3 cameraToV1 = cp_cameraPosition - v1;
    vec3 cameraToV2 = cp_cameraPosition - v2;
    float distanceToV1 = dot(cameraToV1, cameraToV1);
    float distanceToV2 = dot(cameraToV2, cameraToV2);

    if(distanceToV1 < distanceToV2) {
        // v1 closer than v2
        v2 = v2 - deltaNormalized*dr2;
        yDir = orthogonalOnDeltaNormalized;

        vertexPosition = v2 - orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(-da, 1.0);
        EmitVertex();
        vertexPosition = v1 - orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(1.0, 1.0);
        EmitVertex();
        vertexPosition = v2 + orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(-da, -1.0);
        EmitVertex();
        vertexPosition = v1 + orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(1.0, -1.0);
        EmitVertex();
        EndPrimitive();
    } else {
        // v2 closer than v1
        v1 = v1 + deltaNormalized*dr1;
        yDir = -orthogonalOnDeltaNormalized;

        vertexPosition = v1 + orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(-da, 1.0);
        EmitVertex();
        vertexPosition = v2 + orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(1.0, 1.0);
        EmitVertex();
        vertexPosition = v1 - orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(-da, -1.0);
        EmitVertex();
        vertexPosition = v2 - orthogonalOnDeltaNormalizedTimesRadius;
        gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);
        texCoord = vec2(1.0, -1.0);
        EmitVertex();
        EndPrimitive();
    }
}
