layout( points ) in;
layout( triangle_strip, max_vertices = 64 ) out;

in vec3 vs_position[1];
out vec3 position;
out vec3 normal;

uniform float threshold;
uniform float dr;
uniform sampler2D triangleTable;

float eval(vec3 position) {
    $setupShaderNodes();
    return shaderNodeResult;
}

vec3 calculateNormal(vec3 p) {
    // Numerical differentiation with the two point formula:
    // f' ≈ ( f(x+h) - f(x-h) ) / 2h
    float h = 0.01;
    float oneOverTwoH = 1.0 / (2.0*h);

    float fPlusX = eval(vec3(p.x + h, p.y, p.z));
    float fMinusX = eval(vec3(p.x - h, p.y, p.z));

    float fPlusY = eval(vec3(p.x, p.y+h, p.z));
    float fMinusY = eval(vec3(p.x, p.y-h, p.z));

    float fPlusZ = eval(vec3(p.x, p.y, p.z+h));
    float fMinusZ = eval(vec3(p.x, p.y, p.z-h));

    vec3 normal;
    normal.x = (fPlusX - fMinusX) * oneOverTwoH;
    normal.y = (fPlusY - fMinusY) * oneOverTwoH;
    normal.z = (fPlusZ - fMinusZ) * oneOverTwoH;
    normal = normalize(normal);
    return normal;
}

int triTableValue(int i, int j){
    return int(texelFetch(triangleTable, ivec2(i, j), 0).r * 255) - 1;
}

struct GridCell {
    vec3 p[8];
    float val[8];
};

/*
   Linearly interpolate the position where an isosurface cuts
   an edge between two vertices, each with their own scalar value
*/
vec3 linterp(float threshold, vec3 p1, vec3 p2, float valp1, float valp2)
{
    vec3 p;

    if (abs(threshold-valp1) < 0.00001)
        return(p1);
    if (abs(threshold-valp2) < 0.00001)
        return(p2);
    if (abs(valp1-valp2) < 0.00001)
        return(p1);
    float mu = (threshold - valp1) / (valp2 - valp1);
    p = p1 + mu*(p2-p1);

    return(p);
}

void main(void) {
    //vec3 v_000 = vs_position[0] + cp_cameraPosition;                        // Corresponds to vertex 0
    vec3 v_000 = vs_position[0];                        // Corresponds to vertex 0
    vec3 v_001 = v_000 + vec3(0.0, 0.0, dr);       // Corresponds to vertex 4
    vec3 v_011 = v_000 + vec3(0.0, dr, dr);           // Corresponds to vertex 5
    vec3 v_010 = v_000 + vec3(0.0, dr, 0.0);       // Corresponds to vertex 1
    vec3 v_110 = v_000 + vec3(dr, dr, 0.0);           // Corresponds to vertex 2
    vec3 v_111 = v_000 + dr;                         // Corresponds to vertex 6
    vec3 v_101 = v_000 + vec3(dr, 0.0, dr);   // Corresponds to vertex 7
    vec3 v_100 = v_000 + vec3(dr, 0.0, 0.0);       // Corresponds to vertex 3


    GridCell grid;
    grid.p[0] = v_000;
    grid.p[1] = v_010;
    grid.p[2] = v_110;
    grid.p[3] = v_100;
    grid.p[4] = v_001;
    grid.p[5] = v_011;
    grid.p[6] = v_111;
    grid.p[7] = v_101;

    for(int i=0; i<8; i++) {
        grid.val[i] = eval(grid.p[i]);
    }

    int cubeindex = 0;

    if (grid.val[0] < threshold) cubeindex |= 1;
    if (grid.val[1] < threshold) cubeindex |= 2;
    if (grid.val[2] < threshold) cubeindex |= 4;
    if (grid.val[3] < threshold) cubeindex |= 8;
    if (grid.val[4] < threshold) cubeindex |= 16;
    if (grid.val[5] < threshold) cubeindex |= 32;
    if (grid.val[6] < threshold) cubeindex |= 64;
    if (grid.val[7] < threshold) cubeindex |= 128;

    vec3 vertlist[12];
    /* Find the vertices where the surface intersects the cube */
    vertlist[0] = linterp(threshold,grid.p[0],grid.p[1],grid.val[0],grid.val[1]);
    vertlist[1] = linterp(threshold,grid.p[1],grid.p[2],grid.val[1],grid.val[2]);
    vertlist[2] = linterp(threshold,grid.p[2],grid.p[3],grid.val[2],grid.val[3]);
    vertlist[3] = linterp(threshold,grid.p[3],grid.p[0],grid.val[3],grid.val[0]);
    vertlist[4] = linterp(threshold,grid.p[4],grid.p[5],grid.val[4],grid.val[5]);
    vertlist[5] = linterp(threshold,grid.p[5],grid.p[6],grid.val[5],grid.val[6]);
    vertlist[6] = linterp(threshold,grid.p[6],grid.p[7],grid.val[6],grid.val[7]);
    vertlist[7] = linterp(threshold,grid.p[7],grid.p[4],grid.val[7],grid.val[4]);
    vertlist[8] = linterp(threshold,grid.p[0],grid.p[4],grid.val[0],grid.val[4]);
    vertlist[9] = linterp(threshold,grid.p[1],grid.p[5],grid.val[1],grid.val[5]);
    vertlist[10] = linterp(threshold,grid.p[2],grid.p[6],grid.val[2],grid.val[6]);
    vertlist[11] = linterp(threshold,grid.p[3],grid.p[7],grid.val[3],grid.val[7]);

    /* Emit triangles*/
    int triangleStartIndex = cubeindex;
    for (int i=0; triTableValue(cubeindex, i) != -1; i+=3) {
        vec3 p1 = vertlist[triTableValue(cubeindex, i)];
        vec4 mvp_p1 = cp_modelViewProjectionMatrix*vec4(p1, 1.0);
        vec3 n1 = calculateNormal(p1);
        vec3 p2 = vertlist[triTableValue(cubeindex, i+1)];
        vec4 mvp_p2 = cp_modelViewProjectionMatrix*vec4(p2, 1.0);
        vec3 n2 = calculateNormal(p2);
        vec3 p3 = vertlist[triTableValue(cubeindex, i+2)];
        vec4 mvp_p3 = cp_modelViewProjectionMatrix*vec4(p3, 1.0);
        vec3 n3 = calculateNormal(p3);

        // front face
        position = p1;
        normal = n1;
        gl_Position = mvp_p1;
        EmitVertex();

        position = p2;
        normal = n2;
        gl_Position = mvp_p2;
        EmitVertex();

        position = p3;
        normal = n3;
        gl_Position = mvp_p3;
        EmitVertex();
        EndPrimitive();

        // back face
        position = p3;
        normal = -n3;
        gl_Position = mvp_p3;
        EmitVertex();

        position = p2;
        normal = -n2;
        gl_Position = mvp_p2;
        EmitVertex();

        position = p1;
        normal = -n1;
        gl_Position = mvp_p1;
        EmitVertex();
        EndPrimitive();
    }
}
