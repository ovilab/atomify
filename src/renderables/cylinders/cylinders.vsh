layout(location=0) in vec3 vertex1Position;
layout(location=1) in vec3 vertex2Position;
out vec3 vs_vertex1Position;
out vec3 vs_vertex2Position;

void main(void)
{
    gl_Position = vec4(1.0);
    vs_vertex1Position = vertex1Position;
    vs_vertex2Position = vertex2Position;
}
