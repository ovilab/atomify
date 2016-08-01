import QtQuick 2.5
import QtQuick.Controls 1.4
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Extras 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import SimVis.ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Scene3D {
    id: root
    property alias visualizer: visualizer
    property alias simulator: simulator
    // property alias rdf: rdf
    property real scale: 0.23
    property bool addPeriodicCopies: false
    property alias ambientOcclusion: ambientOcclusion
    property alias finalShaderBuilder: finalShaderBuilder
    property alias sphereScale: colorModifier.scale
    property real bondRadius: 0.1
    property alias periodicImages: periodicImages
    onBondRadiusChanged: {
        if(simulator != undefined) {
            simulator.system.atoms.bondRadius = bondRadius
        }
    }

    aspects: ["render", "input", "logic"]

    Entity {
        id: visualizer
        property Camera camera: Camera {
            id: mainCamera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 50
            aspectRatio: root.width / root.height
            nearPlane : 2.0
            farPlane : 500.0
            position: Qt.vector3d(7.0, 7.0, 50.0)
            upVector: Qt.vector3d(0.0, 1.0, 0.0)
            viewCenter: Qt.vector3d(7.0, 7.0, 7.0)
            onPositionChanged: {
                if(simulator != undefined) {
                    simulator.system.cameraPosition = position
                }
            }
        }
        property Spheres spheres: spheres

        components: [
            RenderSettings {

                activeFrameGraph: Viewport {
                    normalizedRect: Qt.rect(0.0, 0.0, 1.0, 1.0)

                    TechniqueFilter {
                        matchAll: FilterKey { name: "renderingStyle"; value: "deferred" }
                        RenderSurfaceSelector {
                            RenderPassFilter {
                                id : geometryPass
                                matchAny : FilterKey { name : "pass"; value : "geometry" }
                                RenderTargetSelector {
                                    target: RenderTarget {
                                        attachments : [
                                            RenderTargetOutput {
                                                objectName : "normalOut"
                                                attachmentPoint : RenderTargetOutput.Color0
                                                texture : Texture2D {
                                                    id : normalTexture
                                                    width : root.width
                                                    height : root.width // TODO use height?
                                                    format : Texture.RGBA32F
                                                    generateMipMaps : false
                                                    magnificationFilter : Texture.Nearest
                                                    minificationFilter : Texture.Nearest
                                                    wrapMode {
                                                        x: WrapMode.ClampToEdge
                                                        y: WrapMode.ClampToEdge
                                                    }
                                                }
                                            },
                                            RenderTargetOutput {
                                                objectName : "positionOut"
                                                attachmentPoint : RenderTargetOutput.Color1
                                                texture : Texture2D {
                                                    id : positionTexture
                                                    width : root.width
                                                    height : root.width // TODO use height?
                                                    format : Texture.RGBA32F
                                                    generateMipMaps : false
                                                    magnificationFilter : Texture.Nearest
                                                    minificationFilter : Texture.Nearest
                                                    wrapMode {
                                                        x: WrapMode.ClampToEdge
                                                        y: WrapMode.ClampToEdge
                                                    }
                                                }
                                            },
                                            RenderTargetOutput {
                                                objectName : "colorOut"
                                                attachmentPoint : RenderTargetOutput.Color2
                                                texture : Texture2D {
                                                    id : colorTexture
                                                    width : root.width
                                                    height : root.width // TODO use height?
                                                    format : Texture.RGBA32F
                                                    generateMipMaps : false
                                                    magnificationFilter : Texture.Nearest
                                                    minificationFilter : Texture.Nearest
                                                    wrapMode {
                                                        x: WrapMode.ClampToEdge
                                                        y: WrapMode.ClampToEdge
                                                    }
                                                }
                                            },
                                            RenderTargetOutput {
                                                objectName: "depth"
                                                attachmentPoint: RenderTargetOutput.Depth
                                                texture: Texture2D {
                                                    id: depthTexture
                                                    width : root.width
                                                    height : root.width // TODO use height?
                                                    format: Texture.D32
                                                    generateMipMaps: false
                                                    magnificationFilter: Texture.Nearest
                                                    minificationFilter: Texture.Nearest
                                                    wrapMode {
                                                        x: WrapMode.ClampToEdge
                                                        y: WrapMode.ClampToEdge
                                                    }
                                                    comparisonFunction: Texture.CompareLessEqual
                                                    comparisonMode: Texture.CompareNone
                                                }
                                            }
                                        ]
                                    }
                                    ClearBuffers {
                                        clearColor: "#000"
                                        buffers: ClearBuffers.ColorDepthBuffer
                                        CameraSelector {
                                            camera: mainCamera
                                        }
                                    }
                                }
                            }
                            RenderPassFilter {
                                matchAny : FilterKey { name : "pass"; value : "ssao" }
                                RenderTargetSelector {
                                    target: RenderTarget {
                                        attachments : [
                                            RenderTargetOutput {
                                                objectName : "ssao"
                                                attachmentPoint : RenderTargetOutput.Color0
                                                texture : Texture2D {
                                                    id : ssaoTexture
                                                    width : 0.5*root.width
                                                    height : 0.5*root.width // TODO use height?
                                                    format : Texture.RGBA32F
                                                    generateMipMaps : false
                                                    magnificationFilter : Texture.Linear
                                                    minificationFilter : Texture.Linear
                                                    wrapMode {
                                                        x: WrapMode.ClampToEdge
                                                        y: WrapMode.ClampToEdge
                                                    }
                                                }
                                            }
                                        ]
                                    }
                                    ClearBuffers {
                                        clearColor: "#000"
                                        buffers: ClearBuffers.ColorDepthBuffer
                                        CameraSelector {
                                            camera: mainCamera
                                        }
                                    }
                                }
                            }
                            RenderPassFilter {
                                matchAny : FilterKey { name : "pass"; value : "blur" }
                                RenderTargetSelector {
                                    target: RenderTarget {
                                        attachments : [
                                            RenderTargetOutput {
                                                objectName : "blur"
                                                attachmentPoint : RenderTargetOutput.Color0
                                                texture : Texture2D {
                                                    id : blurTexture
                                                    width : root.width
                                                    height : root.width // TODO use height?
                                                    format : Texture.RGBA32F
                                                    generateMipMaps : false
                                                    magnificationFilter : Texture.Linear
                                                    minificationFilter : Texture.Linear
                                                    wrapMode {
                                                        x: WrapMode.ClampToEdge
                                                        y: WrapMode.ClampToEdge
                                                    }
                                                }
                                            }
                                        ]
                                    }
                                    ClearBuffers {
                                        clearColor: "#000"
                                        buffers: ClearBuffers.ColorDepthBuffer
                                        CameraSelector {
                                            camera: mainCamera
                                        }
                                    }
                                }
                            }
                            RenderPassFilter {
                                matchAny : FilterKey { name : "pass"; value : "final" }
                                ClearBuffers {
                                    clearColor: "#000"
                                    buffers: ClearBuffers.ColorDepthBuffer
                                    CameraSelector {
                                        id: viewCameraSelector
                                        camera: mainCamera
                                    }
                                }
                            }
                        }
                    }
                }
            },
            InputSettings {}
        ]

        PlaneMesh {
            id: quadMesh
            width: 2.0
            height: 2.0
            meshResolution: Qt.size(2, 2)
        }

        Transform { // We rotate the plane so that it faces us
            id: quadTransform
            rotation: fromAxisAndAngle(Qt.vector3d(1, 0, 0), 90)
        }

        Entity {
            id : ssaoQuadEntity

            Material {
                id: ssaoMaterial
                parameters : [
                    Parameter { name: "normalTexture"; value : normalTexture },
                    Parameter { name: "positionTexture"; value : positionTexture },
                    Parameter { name: "colorTexture"; value : colorTexture },
                    Parameter { name: "depthTexture"; value : depthTexture },
                    Parameter { name: "posMin"; value: spheres.posMin },
                    Parameter { name: "posMax"; value: spheres.posMax }
                ]
                effect: Effect {
                    techniques : [
                        Technique {
                            filterKeys: FilterKey {
                                name: "renderingStyle"
                                value: "deferred"
                            }
                            graphicsApiFilter {
                                api: GraphicsApiFilter.OpenGL
                                profile: GraphicsApiFilter.CoreProfile
                                majorVersion: 3
                                minorVersion: 2
                            }
                            renderPasses : RenderPass {
                                filterKeys : FilterKey { name : "pass"; value : "ssao" }
                                shaderProgram : ShaderProgram {
                                    vertexShaderCode: "
#version 410

uniform highp mat4 modelMatrix;

in highp vec4 vertexPosition;
in highp vec2 vertexTexCoord;

out highp vec2 texCoord;

void main()
{
    texCoord = vertexTexCoord;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = modelMatrix * vertexPosition;
}
"
                                    fragmentShaderCode: ssaoShaderBuilder.finalShader
                                }
                            }
                        }
                    ]
                }

                ShaderBuilder {
                    id: ssaoShaderBuilder

                    property ShaderNode position: ShaderNode {
                        type: "vec3"
                        name: "position"
                        result: "position"
                    }
                    property ShaderNode normal: ShaderNode {
                        type: "vec3"
                        name: "normal"
                        result: "normal"
                    }
                    property ShaderNode color: ShaderNode {
                        type: "vec4"
                        name: "color"
                        result: "color"
                    }

                    material: ssaoMaterial
                    source: "
#version 410
"
+(Qt.platform.os=="osx" ? "#define MACOSX" : "")+"
uniform highp sampler2D normalTexture;
uniform highp sampler2D positionTexture;
uniform highp sampler2D colorTexture;
uniform highp sampler2D depthTexture;
uniform float posMin;
uniform float posMax;

uniform highp mat4 inverseProjectionMatrix;
uniform highp mat4 projectionMatrix;
uniform highp mat4 inverseViewMatrix;
uniform highp mat4 mvp;
uniform highp mat4 viewMatrix;
uniform highp mat4 tangentMatrix;
uniform highp mat4 modelView;
uniform highp vec3 eyePosition;

in highp vec2 texCoord;

out highp vec4 fragColor;

#pragma shadernodes header

highp vec3 positionFromDepth(highp float z, highp vec2 texCoord) {
    highp float x = texCoord.x * 2.0 - 1.0;
    highp float y = texCoord.y * 2.0 - 1.0;
    highp vec4 projectedPos = vec4(x, y, z, 1.0f / z);
    highp vec4 positionVS = inverseProjectionMatrix * projectedPos;
    positionVS /= positionVS.w;
    highp vec4 positionModel = inverseViewMatrix * positionVS;
    return positionModel.xyz;
}

void main()
{
    highp vec3 normal = normalize(-1.0 + 2.0 * texture(normalTexture, texCoord).xyz);
    highp float depth = texture(depthTexture, texCoord).x;
    float deltaMaxMin = posMax - posMin;
#ifdef MACOSX
    vec3 position = eyePosition + posMin + texture(colorTexture, texCoord).xyz * deltaMaxMin;
    vec4 color = texture(positionTexture, texCoord);
#else
    vec3 position = eyePosition + posMin + texture(positionTexture, texCoord).xyz * deltaMaxMin;
    vec4 color = texture(colorTexture, texCoord);
#endif

    if(depth > 1.0 - 1e-5) {
        return;
    }

#pragma shadernodes body
}

"

                    outputs: [
                        ShaderOutput {
                            name: "fragColor"
                            type: "vec4"
                            value: AmbientOcclusion {
                                id: ambientOcclusion
                                samples: 64
                                radius: 10
                                depthTexture: depthTexture
                                mode: "hemisphere"
                            }
                        }
                    ]
                }

            }

            components: [
                //                ssaoLayer,
                ssaoMaterial,
                quadMesh,
                quadTransform
            ]
        }

        Entity {
            id : blurQuadEntity

            Material {
                id: blurMaterial
                parameters : [
                    Parameter { name: "ssaoTexture"; value : ssaoTexture },
                    Parameter { name: "depthTexture"; value : depthTexture },
                    Parameter { name: "blurSize"; value : 7 },
                    Parameter { name: "winSize"; value : Qt.size(root.width, root.height) }
                ]
                effect: Effect {
                    techniques : [
                        Technique {
                            filterKeys: FilterKey {
                                name: "renderingStyle"
                                value: "deferred"
                            }
                            graphicsApiFilter {
                                api: GraphicsApiFilter.OpenGL
                                profile: GraphicsApiFilter.CoreProfile
                                majorVersion: 3
                                minorVersion: 2
                            }
                            renderPasses : RenderPass {
                                filterKeys : FilterKey { name : "pass"; value : "blur" }
                                shaderProgram : ShaderProgram {
                                    vertexShaderCode: "
#version 410

uniform highp mat4 modelMatrix;

in highp vec4 vertexPosition;
in highp vec2 vertexTexCoord;

out highp vec2 texCoord;

void main()
{
    float scale = 0.4;
    texCoord = vertexTexCoord;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = modelMatrix * vertexPosition;
}
"
                                    fragmentShaderCode: "
#version 410

uniform highp sampler2D ssaoTexture;
uniform highp sampler2D depthTexture;

uniform highp vec2 winSize;
uniform highp int blurSize;

uniform highp mat4 inverseProjectionMatrix;
uniform highp mat4 projectionMatrix;
uniform highp mat4 inverseViewMatrix;
uniform highp mat4 mvp;
uniform highp mat4 viewMatrix;
uniform highp mat4 tangentMatrix;
uniform highp mat4 modelView;
uniform highp vec3 eyePosition;

in highp vec2 texCoord;

out highp vec4 fragColor;

#pragma shadernodes header

highp float blurLinearizeDepth(highp float z) {
    highp float f=200.0;
    highp float n = 2.0;

    return (2.0 * n) / (f + n - z * (f - n));
}

void main()
{
    highp vec2 scale = vec2(1.0 / winSize.x, 1.0 / winSize.y); // TODO make additional scale as parameter
    highp vec2 blurSizeHalf = vec2(float(blurSize) * 0.5);
    highp float fragDepth = blurLinearizeDepth(texture(depthTexture, texCoord).x);
    highp vec3 result = texture(ssaoTexture, texCoord).rgb;
    highp int sampleCount = 1;
    for(int i = 0; i < blurSize; i++) {
        for(int j = 0; j < blurSize; j++) {
            highp vec2 offset = scale * (vec2(i, j) - blurSizeHalf);
            highp vec3 value = texture(ssaoTexture, texCoord + offset).rgb;
            highp float sampleDepth = blurLinearizeDepth(texture(depthTexture, texCoord + offset).x);
            if(abs(fragDepth - sampleDepth) < 0.0001) { // TODO make into parameter
                result += value;
                sampleCount += 1;
            }
        }
    }
    fragColor = vec4(result / float(sampleCount), 1.0);
}
"
                                }
                            }
                        }
                    ]
                }
            }

            components: [
                blurMaterial,
                quadMesh,
                quadTransform
            ]
        }

        Entity {
            id : finalQuadEntity

            Material {
                id: finalMaterial
                parameters : [
                    Parameter { name: "blurTexture"; value : blurTexture },
                    Parameter { name: "ssaoTexture"; value : ssaoTexture },
                    Parameter { name: "normalTexture"; value : normalTexture },
                    Parameter { name: "positionTexture"; value : positionTexture },
                    Parameter { name: "colorTexture"; value : colorTexture },
                    Parameter { name: "depthTexture"; value : depthTexture },
                    Parameter { name: "winSize"; value : Qt.size(root.width, root.height) },
                    Parameter { name: "posMin"; value: spheres.posMin },
                    Parameter { name: "posMax"; value: spheres.posMax }
                ]
                effect: Effect {
                    techniques : [
                        Technique {
                            filterKeys: FilterKey {
                                name: "renderingStyle"
                                value: "deferred"
                            }
                            graphicsApiFilter {
                                api: GraphicsApiFilter.OpenGL
                                profile: GraphicsApiFilter.CoreProfile
                                majorVersion: 3
                                minorVersion: 2
                            }
                            renderPasses : RenderPass {
                                filterKeys : FilterKey { name : "pass"; value : "final" }
                                shaderProgram : ShaderProgram {
                                    vertexShaderCode: "
#version 410

uniform highp mat4 modelMatrix;

in highp vec4 vertexPosition;
in highp vec2 vertexTexCoord;

out highp vec2 texCoord;

void main()
{
    texCoord = vertexTexCoord;
    texCoord.y = 1.0 - texCoord.y;
    gl_Position = modelMatrix * vertexPosition;
}
"

                                    fragmentShaderCode: finalShaderBuilder.finalShader
                                    // onFragmentShaderCodeChanged: console.log(fragmentShaderCode)
                                }
                            }
                        }
                    ]
                }

                ShaderBuilder {
                    id: finalShaderBuilder
                    function selectOutput(outputName) {
                        if(outputName === "blurMultiply") {
                            output.value = blurMultiply
                        }
                        if(outputName === "ssaoMultiply") {
                            output.value = ssaoMultiply
                        }
                        if(outputName === "blur") {
                            output.value = blurNode
                        }

                        if(outputName === "ssao") {
                            output.value = ssaoNode
                        }
                        if(outputName === "position") {
                            output.value = position
                        }
                        if(outputName === "color") {
                            output.value = color
                        }
                        if(outputName === "normal") {
                            output.value = normal
                        }
                    }

                    property ShaderNode position: ShaderNode {
                        type: "vec3"
                        name: "position"
                        result: "position"
                    }
                    property ShaderNode normal: ShaderNode {
                        type: "vec3"
                        name: "normal"
                        result: "normal"
                    }
                    property ShaderNode ssao: ShaderNode {
                        type: "vec3"
                        name: "ssao"
                        result: "ssao"
                    }
                    property ShaderNode blur: ShaderNode {
                        type: "vec3"
                        name: "blur"
                        result: "blur"
                    }
                    property ShaderNode color: ShaderNode {
                        type: "vec4"
                        name: "color"
                        result: "color"
                    }

                    material: finalMaterial

                    outputs: [
                        ShaderOutput {
                            id: output
                            name: "fragColor"
                            type: "vec4"
                            value: blurMultiply
                            // value: finalShaderBuilder
                        }
                    ]

                    Multiply {
                        id: ssaoMultiply
                        value1: ssaoNode
                        value2: standardMaterial
                    }
                    Multiply {
                        id: blurMultiply
                        value1: blurNode
                        value2: standardMaterial
                    }
                    ShaderNode {
                        id: blurNode
                        property var blur: finalShaderBuilder.blur
                        name: "ambientOcclusion"
                        type: "vec3"
                        result: "$(blur, vec3)"
                    }
                    ShaderNode {
                        id: ssaoNode
                        property var ssao: finalShaderBuilder.ssao
                        name: "ambientOcclusion"
                        type: "vec3"
                        result: "$(ssao, vec3)"
                    }
                    StandardMaterial {
                        id: standardMaterial
                        color: finalShaderBuilder.color
                        lights: [
                            Light {
                                // position: visualizer.camera.position + visualizer.camera.viewVector.normalized().plus(visualizer.camera.upVector.normalized()).plus(visualizer.camera.viewVector.normalized().crossProduct(visualizer.camera.upVector)).normalized().times(10.0)
                                position: visualizer.camera.position.plus(
                                              (visualizer.camera.viewVector.normalized().plus(
                                                   visualizer.camera.upVector.normalized()).plus(
                                                   visualizer.camera.viewVector.crossProduct(visualizer.camera.upVector)).normalized()).times(20))
                                //position: visualizer.camera.position.plus(visualizer.camera.upVector.normalized())
                                strength: 0.4
                                attenuation: 0.0
                            },
                            Light {
                                position: visualizer.camera.position.minus(
                                              (visualizer.camera.viewVector.normalized().plus(
                                                   visualizer.camera.upVector.normalized()).plus(
                                                   visualizer.camera.viewVector.crossProduct(visualizer.camera.upVector)).normalized()).times(10))
                                strength: 0.4
                                attenuation: 0.0
                            }
                        ]
                    }

                    source: "
#version 410
"
+(Qt.platform.os=="osx" ? "#define MACOSX" : "")+"

uniform highp sampler2D blurTexture;
uniform highp sampler2D ssaoTexture;
uniform highp sampler2D normalTexture;
uniform highp sampler2D positionTexture;
uniform highp sampler2D colorTexture;
uniform highp sampler2D depthTexture;
uniform highp vec2 winSize;
uniform float posMin;
uniform float posMax;

uniform highp mat4 inverseProjectionMatrix;
uniform highp mat4 projectionMatrix;
uniform highp mat4 inverseViewMatrix;
uniform highp mat4 mvp;
uniform highp mat4 viewMatrix;
uniform highp mat4 tangentMatrix;
uniform highp mat4 modelView;
uniform highp vec3 eyePosition;

in highp vec2 texCoord;
out highp vec4 fragColor;

#pragma shadernodes header

highp vec3 positionFromDepth(highp float z, highp vec2 texCoord) {
    highp float x = texCoord.x * 2.0 - 1.0;
    highp float y = texCoord.y * 2.0 - 1.0;
    highp vec4 projectedPos = vec4(x, y, z, 1.0f / z);
    highp vec4 positionVS = inverseProjectionMatrix * projectedPos;
    positionVS /= positionVS.w;
    highp vec4 positionModel = inverseViewMatrix * positionVS;
    return positionModel.xyz;
}

void main()
{
        highp vec3 normal = normalize(-1.0 + 2.0 * texture(normalTexture, texCoord).xyz);
        highp float depth = texture(depthTexture, texCoord).x;
        float deltaMaxMin = posMax - posMin;
#ifdef MACOSX
    vec3 position = eyePosition + posMin + texture(colorTexture, texCoord).xyz * deltaMaxMin;
    vec4 color = texture(positionTexture, texCoord);
#else
    vec3 position = eyePosition + posMin + texture(positionTexture, texCoord).xyz * deltaMaxMin;
    vec4 color = texture(colorTexture, texCoord);
#endif
        highp vec3 ssao = texture(ssaoTexture, texCoord).rgb;
        highp vec3 blur = texture(blurTexture, texCoord).rgb;

    if(depth > 1.0 - 1e-5) {
        discard;
    }

#pragma shadernodes body
}

"
                }
            }

            components: [
                //                finalLayer,
                quadMesh,
                quadTransform,
                finalMaterial
            ]
        }

        AtomifySimulator {
            id: simulator
            simulationSpeed: 1
            system.atoms.modifiers: [
                colorModifier,
                periodicImages,
                groupModifier,
                regionModifier
            ]
        }

        DesktopController {
            id: navigationController
            camera: visualizer.camera
            onPressed: {
                root.focus = true
            }
        }

        ColorModifier {
            id: colorModifier
            scale: 0.2
        }

        GroupModifier {
            id: groupModifier
        }

        RegionModifier {
            id: regionModifier
        }

        PeriodicImages {
            id: periodicImages
            numberOfCopiesX: 1
            numberOfCopiesY: 1
            numberOfCopiesZ: 1
        }

        Spheres {
            id: spheres
            camera: visualizer.camera
            sphereData: simulator.system.atoms.sphereData
            // TODO: Is posMin/posMax +-100 ok? We don't need system size anymore since all positions are relative to camera
            posMin: -100
            posMax:  100
        }

        Bonds {
            id: bonds
            bondData: simulator.system.atoms.bondData
            posMin: spheres.posMin
            posMax: spheres.posMax
        }
    }
}
