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
//    float sphereRadius1 = vs_radius1[0]*0.95; // Spheres discard any r^2 > 0.9 => r>0.95
//    float sphereRadius2 = vs_radius2[0]*0.95; // Spheres discard any r^2 > 0.9 => r>0.95
//    float dr1 = sqrt(sphereRadius1*sphereRadius1 - radius*radius);
//    float dr2 = sqrt(sphereRadius2*sphereRadius2 - radius*radius);

    // Move vertices slightly away from camera so they always appear behind spheres
    vec3 v1 = vs_vertex1Position[0];
    vec3 v2 = vs_vertex2Position[0];
    vec4 v14 = vec4(v1, 1.0);
    vec4 v24 = vec4(v2, 1.0);
    vec3 delta = v2 - v1;
    vec3 deltaNormalized = normalize(delta);
    float cosphi = abs(dot(cp_viewVector, deltaNormalized)); // Cheating or being stupid with the factor 0.5
    float sinphi = sqrt(1.0 - cosphi*cosphi);

    vec4 v1_mv = cp_modelViewMatrix*vec4(v1, 1.0);
    vec4 v2_mv = cp_modelViewMatrix*vec4(v2, 1.0);
    float x1 = v1_mv.x; float y1 = v1_mv.y; float z1 = v1_mv.z;
    float x2 = v2_mv.x; float y2 = v2_mv.y; float z2 = v2_mv.z;
    float r1 = sqrt(dot(v1_mv,v1_mv));
    float r2 = sqrt(dot(v2_mv,v2_mv));
    float theta1 = acos(z/r);
    float phi1 = atan(y1,z1);
    float phi2 = atan(y2,z2);


    vec4 delta_mv = v2_mv-v1_mv;
    float delta_mv_length = dot(delta_mv,delta_mv);
    float costheta = delta_mv.x / delta_mv_length;
    float sintheta = delta_mv.y / delta_mv_length;
    vec4 delta_mv_half = 0.5*delta_mv;

    vec4 middle = vec4(0.5*(v2+v1), 1.0);
    da = 0.0;
    float a = 10;
    vertexPosition = v2;
    // gl_Position = cp_modelViewProjectionMatrix*middle + vec4(-delta_mv_half.x + radius*sintheta, -delta_mv_half.y + radius*costheta, 0.0, 0.0);
    gl_Position = cp_modelViewProjectionMatrix*v14 + cp_projectionMatrix*vec4(-radius*sintheta*cosphi, radius*costheta*sinphi, 0.0, 0.0);
    texCoord = vec2(0, 1.0);
    EmitVertex();
    vertexPosition = v1;
    // gl_Position = cp_modelViewProjectionMatrix*middle + vec4(delta_mv_half.x + radius*sintheta, delta_mv_half.y + radius*costheta, 0.0, 0.0);
    gl_Position = cp_modelViewProjectionMatrix*v24 + cp_projectionMatrix*vec4(radius*sintheta*cosphi, radius*costheta*sinphi, 0.0, 0.0);
    texCoord = vec2(1.0, 1.0);
    EmitVertex();
    vertexPosition = v2;
    // gl_Position = cp_modelViewProjectionMatrix*middle + vec4(-delta_mv_half.x - radius*sintheta, -delta_mv_half.y - radius*costheta, 0.0, 0.0);
    gl_Position = cp_modelViewProjectionMatrix*v14 + cp_projectionMatrix*vec4(-radius*sintheta*cosphi, -radius*costheta*sinphi, 0.0, 0.0);
    texCoord = vec2(0, -1.0);
    EmitVertex();
    vertexPosition = v1;
    // gl_Position = cp_modelViewProjectionMatrix*middle + vec4(delta_mv_half.x - radius*sintheta, delta_mv_half.y - radius*costheta, 0.0, 0.0);
    gl_Position = cp_modelViewProjectionMatrix*v24 + cp_projectionMatrix*vec4(radius*sintheta*cosphi, -radius*costheta*sinphi, 0.0, 0.0);
    texCoord = vec2(1.0, -1.0);
    EmitVertex();
    EndPrimitive();
}
