layout( points ) in;
layout( triangle_strip, max_vertices = 4 ) out;
in vec3 vs_color[1];
in float vs_scale[1];
in vec3 vs_vertexPosition[1];

out vec2 texCoord;
out vec3 vertexPosition;
out vec3 color;

void main(void) {
    vec4 pos = gl_in[0].gl_Position;
    vertexPosition = vs_vertexPosition[0];
    float scale = vs_scale[0];
    color = vs_color[0];

    gl_Position = pos + cp_projectionMatrix*vec4(-scale, -scale, 0.0, 0.0);
    texCoord = vec2(-1.0, -1.0);
    EmitVertex();
    gl_Position = pos + cp_projectionMatrix*vec4(-scale, scale, 0.0, 0.0);
    texCoord = vec2(-1.0, 1.0);
    EmitVertex();
    gl_Position = pos + cp_projectionMatrix*vec4(scale, -scale, 0.0, 0.0);
    texCoord = vec2(1.0, -1.0);
    EmitVertex();
    gl_Position = pos + cp_projectionMatrix*vec4(scale, scale, 0.0, 0.0);
    texCoord = vec2(1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}
