layout(location=0) in vec3 vertex1Position;
layout(location=1) in vec3 vertex2Position;
layout(location=2) in float radius1;
layout(location=3) in float radius2;
out vec3 vs_vertex1Position;
out vec3 vs_vertex2Position;
out float vs_radius1;
out float vs_radius2;

void main(void)
{
    gl_Position = vec4(1.0);
    vs_vertex1Position = vertex1Position;
    vs_vertex2Position = vertex2Position;
    vs_radius1 = radius1;
    vs_radius2 = radius2;
}
