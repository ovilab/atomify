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
    vec4 v1mvp = cp_modelViewProjectionMatrix*v14;
    vec4 v2mvp = cp_modelViewProjectionMatrix*v24;
    vec4 delta = v2mvp/v2mvp.w - v1mvp/v1mvp.w;
    vec4 deltaNormalized = normalize(delta);

    vec4 ortho = vec4(cross(vec3(0,0,1), deltaNormalized.xyz), 0.0);

    da = costheta;

    vec4 v = cp_modelViewProjectionMatrix*vec4(0.5*(v1+v2), 1.0);

    gl_Position = v1mvp - ortho*radius;
    texCoord = vec2(0, 1.0);
    EmitVertex();

    gl_Position = v2mvp - ortho*radius;
    texCoord = vec2(1.0, 1.0);
    EmitVertex();

    gl_Position = v1mvp + ortho*radius;
    texCoord = vec2(0, -1.0);
    EmitVertex();

    gl_Position = v2mvp + ortho*radius;
    texCoord = vec2(1.0, -1.0);
    EmitVertex();
    EndPrimitive();
}
