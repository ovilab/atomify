pragma Singleton

import QtQuick 2.0
import ShaderNodes 1.0

QtObject {
    function glslType(value) {
        if(value && value.type) {
            return value.type
        }
        if(ShaderUtils.isList(value)) {
            var highestComponentCount = 0
            var bestType = "float"
            for(var i in value) {
                var listValue = value[i]
                var type = glslType(listValue)
                var componentCount = ShaderUtils.componentCount(listValue)
                if(componentCount > highestComponentCount) {
                    highestComponentCount = componentCount
                    bestType = type
                }
            }
            return bestType
        } else {
            var type = ShaderUtils.glslType(value)
            if(type === "invalid") {
                console.log("Type of " + typeof(value) + " is invalid")
            }
            return type
        }
    }
}
