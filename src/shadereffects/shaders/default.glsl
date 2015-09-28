// BEGIN default.glsl
#ifndef DEFAULTGLSL
#define DEFAULTGLSL
uniform highp mat4 cp_modelViewMatrix;
uniform highp mat4 cp_projectionMatrix;
uniform highp mat4 cp_modelViewMatrixInverse;
uniform highp mat4 cp_projectionMatrixInverse;
uniform highp mat4 cp_modelViewProjectionMatrix;
uniform highp vec3 cp_cameraPosition;
uniform highp vec3 cp_viewVector;
uniform highp vec3 cp_upVector;
uniform highp vec3 cp_rightVector;
uniform highp float cp_time;

#endif
// END default.glsl
