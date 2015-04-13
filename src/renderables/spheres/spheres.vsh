// BEGIN spheres.vsh
attribute highp vec4 a_position;
attribute highp vec3 a_color;
attribute highp vec2 a_texcoord;
attribute highp float a_sphereId;
varying highp vec2 coords;
varying highp vec3 color;
varying highp vec3 vertexPosition;
varying highp float sphereId;

void main() {
    gl_Position = cp_modelViewProjectionMatrix*a_position;
    vertexPosition = a_position.xyz;
    sphereId = a_sphereId;

    coords = a_texcoord;
    color = a_color;
}
// END spheres.vsh
