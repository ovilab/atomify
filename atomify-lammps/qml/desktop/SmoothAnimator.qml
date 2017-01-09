import QtQuick 2.5
import Qt3D.Core 2.0
import Qt3D.Render 2.0
import Qt3D.Input 2.0
import Qt3D.Logic 2.0
import QtQml 2.2
import Atomify 1.0

QtObject {
    id: root
    
    signal stepped(real dt)

    property real duration: 0.4
    readonly property real velocity: d.velocity

    function increase(delta) {
        d.velocity += delta
        d.timeLeft = duration
    }

    readonly property QtObject __internal: QtObject {
        id: d

        property real velocity: 0
        property real timeLeft: 0
    }
    
    function step(dt) {
        if(dt <= 0) {
            return
        }

        if(d.timeLeft <= 0) {
            d.velocity = 0.0
            return
        }
        
        var stepsLeft = Math.ceil(d.timeLeft / dt)
        d.velocity -= d.velocity / stepsLeft
        d.timeLeft -= dt
        
        stepped(dt)
    }
}
