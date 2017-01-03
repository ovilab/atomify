import ShaderNodes 1.0
import ShaderNodes 1.0 as Nodes

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick 2.0 as QQ2
import QtQuick.Scene3D 2.0

ShaderProgram {
    vertexShaderCode:
        "#version 330
in vec4 vertexPosition;
in vec3 vertexNormal;
out vec3 normal0;
out vec3 position0;
uniform mat4 mvp;
uniform mat4 modelMatrix;
uniform mat4 modelView;
uniform mat3 modelNormalMatrix;
uniform mat3 modelViewNormal;
uniform vec4 meshColor;
void main()
{
    position0 = (modelMatrix * vec4(vertexPosition.xyz, 1.0)).xyz ;
    normal0 = modelNormalMatrix * vertexNormal;
    gl_Position = mvp * vertexPosition;
}"
    fragmentShaderCode: deferredFragmentShaderBuilder.finalShader
}
