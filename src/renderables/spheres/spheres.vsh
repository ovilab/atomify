// BEGIN spheres.vsh
attribute highp vec3 a_position;
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
    // vertexPosition = a_position.xyz + 0.5*(cp_rightVector*coords.x + cp_upVector*coords.y);

    vertexPosition = a_position.xyz;
    vertexPosition -= cp_upPlusRightHalf*(a_scale*float(a_vertexId==0.0));
    vertexPosition -= cp_upMinusRightHalf*(a_scale*float(a_vertexId==1.0));
    vertexPosition += cp_upPlusRightHalf*(a_scale*float(a_vertexId==2.0));
    vertexPosition += cp_upMinusRightHalf*(a_scale*float(a_vertexId==3.0));

    gl_Position = cp_modelViewProjectionMatrix*vec4(vertexPosition, 1.0);

    sphereId = a_sphereId;

    color = a_color;
}
// END spheres.vsh
