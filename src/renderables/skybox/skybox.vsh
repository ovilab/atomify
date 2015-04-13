// BEGIN skybox.vsh
uniform highp mat4 modelViewMatrixPrimeInverted;
attribute highp vec2 a_position;
varying highp vec3 textureCoordinate;

void main() {
//    gl_Position = cp_modelViewProjectionMatrix*vec4(a_position, 1.0);
//    textureCoordinate = a_position;
    vec4 pos = vec4(a_position, 0.0, 1.0);
    gl_Position = pos;
    textureCoordinate = (modelViewMatrixPrimeInverted*cp_projectionMatrixInverse*vec4(a_position, 0.0, 1.0)).xyz;
}
// END skybox.vsh
