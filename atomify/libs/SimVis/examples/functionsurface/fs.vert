layout(location=0) in vec3 in_position;

out vec3 vs_position;

void main(void)
{
    vs_position = in_position;

    gl_Position = cp_modelViewProjectionMatrix * vec4(in_position, 1.0);
}
