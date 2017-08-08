import qbs 1.0

Product {
    name: "net.ovilab.SimVis"
    type: "staticlibrary"

    cpp.includePaths: ["src"]
    cpp.cxxLanguageVersion: "c++14"

    Depends { name: 'cpp' }
    Depends {
        name: "Qt"
        submodules: ["core", "quick", "3dcore", "3drender", "3dinput", "3dquick"]
    }

    Export {
        cpp.includePaths: ["src"]
        Depends { name: "cpp" }
    }

    files: [
        "package_vendor.pri",
        "src/simvis_shaders.qrc",
        "src/imports/simvis_imports.qrc",
        "src/core/simulator.cpp",
        "src/core/simvisplugin.cpp",
        "src/shadernodes/shadernode.cpp",
        "src/shadernodes/shaderbuilder.cpp",
        "src/shadernodes/shaderoutput.cpp",
        "src/shadernodes/shaderbuilderbinding.cpp",
        "src/shadernodes/shaderutils.cpp",
        "src/render/geometry/pointgeometry.cpp",
        "src/render/geometry/spherespointgeometry.cpp",
        "src/render/geometry/spheredata.cpp",
        "src/render/geometry/cylinderdata.cpp",
        "src/render/geometry/uniformgridgeometry.cpp",
        "src/utils/marchingcubestabletexture.cpp",
        "src/core/simulator.h",
        "src/core/simvisplugin.h",
        "src/SimVis/Simulator",
        "src/shadernodes/shadernode.h",
        "src/shadernodes/shaderbuilder.h",
        "src/shadernodes/shaderoutput.h",
        "src/shadernodes/shaderutils.h",
        "src/shadernodes/shaderbuilderbinding.h",
        "src/render/geometry/pointgeometry.h",
        "src/render/geometry/spherespointgeometry.h",
        "src/render/geometry/spheredata.h",
        "src/SimVis/SphereData",
        "src/render/geometry/cylinderdata.h",
        "src/render/geometry/uniformgridgeometry.h",
        "src/SimVis/CylinderData",
        "src/utils/marchingcubestabletexture.h",
    ]
}
