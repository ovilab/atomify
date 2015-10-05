//layout( points ) in;
//layout( triangle_strip, max_vertices = 4 ) out;
//uniform float radius;
//in vec3 vs_vertex1Position[1];
//in vec3 vs_vertex2Position[1];
//in float vs_radius1[1];
//in float vs_radius2[1];

//out vec2 texCoord;
//out vec3 vertexPosition;
//out vec3 yDir;
//out float da;

//void main(void) {
//    float sphereRadius1 = vs_radius1[0]; // Spheres discard any r^2 > 0.9 => r>0.95
//    float sphereRadius2 = vs_radius2[0]; // Spheres discard any r^2 > 0.9 => r>0.95
//    float dr1 = sqrt(sphereRadius1*sphereRadius1 - radius*radius);
//    float dr2 = sqrt(sphereRadius2*sphereRadius2 - radius*radius);
//    vec3 epsilonAwayFromCamera = cp_viewVector*0.001;

//    // Move vertices slightly away from camera so they always appear behind spheres
//    vec3 v1 = vs_vertex1Position[0]+epsilonAwayFromCamera;
//    vec3 v2 = vs_vertex2Position[0]+epsilonAwayFromCamera;
//    vec3 deltaV2V1Normalized = normalize(v2-v1);

//    vec4 v14 = vec4(v1+deltaV2V1Normalized*dr1, 1.0);
//    vec4 v24 = vec4(v2-deltaV2V1Normalized*dr2, 1.0);
//    vec4 v1mvp = cp_modelViewProjectionMatrix*v14;
//    vec4 v2mvp = cp_modelViewProjectionMatrix*v24;

//    vec4 delta = v2mvp/v2mvp.w - v1mvp/v1mvp.w;
//    float costheta = normalize(delta).z;
//    vec4 deltaNormalized = normalize(delta);

//    vec4 ortho = vec4(cross(vec3(0,0,1), deltaNormalized.xyz), 0.0);

//    // float costheta = abs(dot(cp_viewVector, deltaV2V1Normalized)); // Cheating or being stupid with the factor 0.5
//    //float costheta = deltaNormalized.x;
//    da = radius*costheta;

//    vec3 cameraToV1 = cp_cameraPosition - v1;
//    vec3 cameraToV2 = cp_cameraPosition - v2;
//    float distanceToV1 = dot(cameraToV1, cameraToV1);
//    float distanceToV2 = dot(cameraToV2, cameraToV2);
//    if(distanceToV1 > distanceToV2) {
//        // v2 closer than v1
//        gl_Position = v1mvp - ortho*radius;
//        texCoord = vec2(-da, 1.0);
//        EmitVertex();

//        gl_Position = v2mvp - ortho*radius;
//        texCoord = vec2(1.0, 1.0);
//        EmitVertex();

//        gl_Position = v1mvp + ortho*radius;
//        texCoord = vec2(-da, -1.0);
//        EmitVertex();

//        gl_Position = v2mvp + ortho*radius;
//        texCoord = vec2(1.0, -1.0);
//        EmitVertex();
//        EndPrimitive();
//    } else {
//        // v1 closer than v2
//        gl_Position = v1mvp - ortho*radius;
//        texCoord = vec2(1.0, 1.0);
//        EmitVertex();

//        gl_Position = v2mvp - ortho*radius;
//        texCoord = vec2(-da, 1.0);
//        EmitVertex();

//        gl_Position = v1mvp + ortho*radius;
//        texCoord = vec2(1.0, -1.0);
//        EmitVertex();

//        gl_Position = v2mvp + ortho*radius;
//        texCoord = vec2(-da, -1.0);
//        EmitVertex();
//        EndPrimitive();
//    }
//}





