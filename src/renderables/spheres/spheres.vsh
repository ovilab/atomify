// BEGIN spheres.vsh
attribute highp vec4 a_position;
attribute highp vec3 a_color;
attribute highp vec2 a_texcoord;
attribute highp float a_sphereId;
attribute highp float a_scale;
attribute highp float a_vertexId;

uniform highp vec3 cp_upPlusRightHalf;
uniform highp vec3 cp_upMinusRightHalf;
varying highp vec2 coords;
varying highp vec3 color;
varying highp vec3 vertexPosition;
varying highp float sphereId;

void main() {
    coords = 2.0*a_texcoord - 1.0;
    vertexPosition = a_position.xyz;

    if(a_vertexId==0.0) vertexPosition -= cp_upPlusRightHalf*a_scale;
    if(a_vertexId==1.0) vertexPosition -= cp_upMinusRightHalf*a_scale;
    if(a_vertexId==2.0) vertexPosition += cp_upPlusRightHalf*a_scale;
    if(a_vertexId==3.0) vertexPosition += cp_upMinusRightHalf*a_scale;

    gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);

    sphereId = a_sphereId;

    color = a_color;
}
// END spheres.vsh
