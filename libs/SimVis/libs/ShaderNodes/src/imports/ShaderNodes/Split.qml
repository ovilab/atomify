import QtQuick 2.0
import ShaderNodes 1.0

ShaderNode {
    id: splitRoot
    property var input

    property var a: ShaderNode {
        name: "split_a"
        type: "float"
        dependencies: [splitRoot]
    }
    property var b: ShaderNode {
        name: "split_b"
        type: "float"
        dependencies: [splitRoot]
    }

    exportedTypeName: "Split"

    name: "split"
    type: a.type

    source: "
float tmpval = sqrt($(input, float));
$a = tmpval * 2.0;
$b = tmpval * 4.0;
"
    result: "$a"
}
