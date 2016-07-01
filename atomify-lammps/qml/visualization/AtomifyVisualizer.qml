import QtQuick 2.5
import QtQuick.Controls 1.4
import Atomify 1.0

import Qt3D.Core 2.0
import Qt3D.Render 2.0

import QtQuick.Scene3D 2.0

import SimVis 1.0
import SimVis.ShaderNodes 1.0

import "../desktop" // TODO should be separate controllers for desktop and mobile

Scene3D {
    id: atomifyVisualizerRoot
    property alias visualizer: visualizer
    property alias simulator: simulator
    // property alias rdf: rdf
    property real scale: 0.23
    property bool addPeriodicCopies: false

    aspects: ["render", "input", "logic"]

    Visualizer {
        id: visualizer
        property Spheres spheres: spheres

        clearColor: "#012"
        camera.aspectRatio: atomifyVisualizerRoot.width / atomifyVisualizerRoot.height

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
            enabled: false
            numberOfCopiesX: 1
            numberOfCopiesY: 1
            numberOfCopiesZ: 1
        }

        Spheres {
            id: spheres
            camera: visualizer.camera
            sphereData: simulator.system.atoms.sphereData
            fragmentColor: StandardMaterial {
                id: spheresFragColor
                lights: [
                    Light {
                        position: visualizer.camera.position
                        attenuation: 0.1
                    }
                ]
                color: spheres.fragmentBuilder.color
                ambientIntensity: 2.0
            }
        }

        Bonds {
            id: bonds
            bondData: simulator.system.atoms.bondData
            fragmentColor: StandardMaterial {
                lights: [
                    Light {
                        position: visualizer.camera.position
                        attenuation: 0.1
                    }
                ]
                ambientIntensity: 2.0
            }
        }
    }
}
