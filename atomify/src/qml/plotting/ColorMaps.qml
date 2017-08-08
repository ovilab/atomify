import QtQuick 2.7
import QtDataVisualization 1.2

Item {
    property ColorGradient jet: ColorGradient {
    ColorGradientStop { position: 0.0 ; color: Qt.rgba(0.0, 0.0, 0.392156863, 1.0) }
    ColorGradientStop { position: 0.15; color: Qt.rgba(0.0, 0.196078431, 1.0, 1.0) }
    ColorGradientStop { position: 0.35; color: Qt.rgba(0.0, 1.0, 1.0, 1.0) }
    ColorGradientStop { position: 0.65; color: Qt.rgba(1.0, 1.0, 0.0, 1.0) }
    ColorGradientStop { position: 0.85; color: Qt.rgba(1.0, 0.117647059, 0.0, 1.0) }
    ColorGradientStop { position: 1.00; color: Qt.rgba(1.0, 0.0,  0.0, 1.0) }
    }

    property Gradient jet_: Gradient {
    GradientStop { position: 0.0 ; color: Qt.rgba(0.0, 0.0, 0.392156863, 1.0) }
    GradientStop { position: 0.15; color: Qt.rgba(0.0, 0.196078431, 1.0, 1.0) }
    GradientStop { position: 0.35; color: Qt.rgba(0.0, 1.0, 1.0, 1.0) }
    GradientStop { position: 0.65; color: Qt.rgba(1.0, 1.0, 0.0, 1.0) }
    GradientStop { position: 0.85; color: Qt.rgba(1.0, 0.117647059, 0.0, 1.0) }
    GradientStop { position: 1.00; color: Qt.rgba(1.0, 0.0, 0.0, 1.0) }
    }

    property ColorGradient magma: ColorGradient {
    ColorGradientStop { position: 0.0; color: Qt.rgba(0.001462, 0.000466, 0.013866, 1.0) }
    ColorGradientStop { position: 0.1015625; color: Qt.rgba(0.083446, 0.056225, 0.220755, 1.0) }
    ColorGradientStop { position: 0.203125; color: Qt.rgba(0.238826, 0.059517, 0.443256, 1.0) }
    ColorGradientStop { position: 0.30078125; color: Qt.rgba(0.396467, 0.102902, 0.502658, 1.0) }
    ColorGradientStop { position: 0.40234375; color: Qt.rgba(0.556571, 0.163269, 0.50523, 1.0) }
    ColorGradientStop { position: 0.5; color: Qt.rgba(0.716387, 0.214982, 0.47529, 1.0) }
    ColorGradientStop { position: 0.6015625; color: Qt.rgba(0.874176, 0.291859, 0.406205, 1.0) }
    ColorGradientStop { position: 0.703125; color: Qt.rgba(0.96968, 0.446936, 0.360311, 1.0) }
    ColorGradientStop { position: 0.80078125; color: Qt.rgba(0.995122, 0.631696, 0.431951, 1.0) }
    ColorGradientStop { position: 0.90234375; color: Qt.rgba(0.995424, 0.819875, 0.57914, 1.0) }
    ColorGradientStop { position: 1.0; color: Qt.rgba(0.987053, 0.991438, 0.749504, 1.0) }
    }
    property Gradient magma_: Gradient {
    GradientStop { position: 0.0; color: Qt.rgba(0.001462, 0.000466, 0.013866, 1.0) }
    GradientStop { position: 0.1015625; color: Qt.rgba(0.083446, 0.056225, 0.220755, 1.0) }
    GradientStop { position: 0.203125; color: Qt.rgba(0.238826, 0.059517, 0.443256, 1.0) }
    GradientStop { position: 0.30078125; color: Qt.rgba(0.396467, 0.102902, 0.502658, 1.0) }
    GradientStop { position: 0.40234375; color: Qt.rgba(0.556571, 0.163269, 0.50523, 1.0) }
    GradientStop { position: 0.5; color: Qt.rgba(0.716387, 0.214982, 0.47529, 1.0) }
    GradientStop { position: 0.6015625; color: Qt.rgba(0.874176, 0.291859, 0.406205, 1.0) }
    GradientStop { position: 0.703125; color: Qt.rgba(0.96968, 0.446936, 0.360311, 1.0) }
    GradientStop { position: 0.80078125; color: Qt.rgba(0.995122, 0.631696, 0.431951, 1.0) }
    GradientStop { position: 0.90234375; color: Qt.rgba(0.995424, 0.819875, 0.57914, 1.0) }
    GradientStop { position: 1.0; color: Qt.rgba(0.987053, 0.991438, 0.749504, 1.0) }
    }
    property ColorGradient inferno: ColorGradient {
    ColorGradientStop { position: 0.0; color: Qt.rgba(0.001462, 0.000466, 0.013866, 1.0) }
    ColorGradientStop { position: 0.1015625; color: Qt.rgba(0.09299, 0.045583, 0.234358, 1.0) }
    ColorGradientStop { position: 0.203125; color: Qt.rgba(0.26481, 0.039647, 0.409345, 1.0) }
    ColorGradientStop { position: 0.30078125; color: Qt.rgba(0.422549, 0.092501, 0.432714, 1.0) }
    ColorGradientStop { position: 0.40234375; color: Qt.rgba(0.584521, 0.150294, 0.402385, 1.0) }
    ColorGradientStop { position: 0.5; color: Qt.rgba(0.735683, 0.215906, 0.330245, 1.0) }
    ColorGradientStop { position: 0.6015625; color: Qt.rgba(0.869409, 0.321827, 0.221482, 1.0) }
    ColorGradientStop { position: 0.703125; color: Qt.rgba(0.956852, 0.475356, 0.094695, 1.0) }
    ColorGradientStop { position: 0.80078125; color: Qt.rgba(0.987819, 0.652773, 0.045581, 1.0) }
    ColorGradientStop { position: 0.90234375; color: Qt.rgba(0.962517, 0.851476, 0.285546, 1.0) }
    ColorGradientStop { position: 1.0; color: Qt.rgba(0.988362, 0.998364, 0.644924, 1.0) }
    }
    property Gradient inferno_: Gradient {
    GradientStop { position: 0.0; color: Qt.rgba(0.001462, 0.000466, 0.013866, 1.0) }
    GradientStop { position: 0.1015625; color: Qt.rgba(0.09299, 0.045583, 0.234358, 1.0) }
    GradientStop { position: 0.203125; color: Qt.rgba(0.26481, 0.039647, 0.409345, 1.0) }
    GradientStop { position: 0.30078125; color: Qt.rgba(0.422549, 0.092501, 0.432714, 1.0) }
    GradientStop { position: 0.40234375; color: Qt.rgba(0.584521, 0.150294, 0.402385, 1.0) }
    GradientStop { position: 0.5; color: Qt.rgba(0.735683, 0.215906, 0.330245, 1.0) }
    GradientStop { position: 0.6015625; color: Qt.rgba(0.869409, 0.321827, 0.221482, 1.0) }
    GradientStop { position: 0.703125; color: Qt.rgba(0.956852, 0.475356, 0.094695, 1.0) }
    GradientStop { position: 0.80078125; color: Qt.rgba(0.987819, 0.652773, 0.045581, 1.0) }
    GradientStop { position: 0.90234375; color: Qt.rgba(0.962517, 0.851476, 0.285546, 1.0) }
    GradientStop { position: 1.0; color: Qt.rgba(0.988362, 0.998364, 0.644924, 1.0) }
    }
    property ColorGradient viridis: ColorGradient {
    ColorGradientStop { position: 0.0; color: Qt.rgba(0.267004, 0.004874, 0.329415, 1.0) }
    ColorGradientStop { position: 0.1015625; color: Qt.rgba(0.28229, 0.145912, 0.46151, 1.0) }
    ColorGradientStop { position: 0.203125; color: Qt.rgba(0.252194, 0.269783, 0.531579, 1.0) }
    ColorGradientStop { position: 0.30078125; color: Qt.rgba(0.204903, 0.375746, 0.553533, 1.0) }
    ColorGradientStop { position: 0.40234375; color: Qt.rgba(0.162142, 0.474838, 0.55814, 1.0) }
    ColorGradientStop { position: 0.5; color: Qt.rgba(0.127568, 0.566949, 0.550556, 1.0) }
    ColorGradientStop { position: 0.6015625; color: Qt.rgba(0.137339, 0.662252, 0.515571, 1.0) }
    ColorGradientStop { position: 0.703125; color: Qt.rgba(0.274149, 0.751988, 0.436601, 1.0) }
    ColorGradientStop { position: 0.80078125; color: Qt.rgba(0.487026, 0.823929, 0.312321, 1.0) }
    ColorGradientStop { position: 0.90234375; color: Qt.rgba(0.751884, 0.874951, 0.143228, 1.0) }
    ColorGradientStop { position: 1.0; color: Qt.rgba(0.993248, 0.906157, 0.143936, 1.0) }
    }
    property Gradient viridis_: Gradient {
    GradientStop { position: 0.0; color: Qt.rgba(0.267004, 0.004874, 0.329415, 1.0) }
    GradientStop { position: 0.1015625; color: Qt.rgba(0.28229, 0.145912, 0.46151, 1.0) }
    GradientStop { position: 0.203125; color: Qt.rgba(0.252194, 0.269783, 0.531579, 1.0) }
    GradientStop { position: 0.30078125; color: Qt.rgba(0.204903, 0.375746, 0.553533, 1.0) }
    GradientStop { position: 0.40234375; color: Qt.rgba(0.162142, 0.474838, 0.55814, 1.0) }
    GradientStop { position: 0.5; color: Qt.rgba(0.127568, 0.566949, 0.550556, 1.0) }
    GradientStop { position: 0.6015625; color: Qt.rgba(0.137339, 0.662252, 0.515571, 1.0) }
    GradientStop { position: 0.703125; color: Qt.rgba(0.274149, 0.751988, 0.436601, 1.0) }
    GradientStop { position: 0.80078125; color: Qt.rgba(0.487026, 0.823929, 0.312321, 1.0) }
    GradientStop { position: 0.90234375; color: Qt.rgba(0.751884, 0.874951, 0.143228, 1.0) }
    GradientStop { position: 1.0; color: Qt.rgba(0.993248, 0.906157, 0.143936, 1.0) }
    }
    property ColorGradient plasma: ColorGradient {
    ColorGradientStop { position: 0.0; color: Qt.rgba(0.050383, 0.029803, 0.527975, 1.0) }
    ColorGradientStop { position: 0.1015625; color: Qt.rgba(0.261183, 0.013308, 0.617911, 1.0) }
    ColorGradientStop { position: 0.203125; color: Qt.rgba(0.423689, 0.000646, 0.658956, 1.0) }
    ColorGradientStop { position: 0.30078125; color: Qt.rgba(0.568201, 0.055778, 0.639477, 1.0) }
    ColorGradientStop { position: 0.40234375; color: Qt.rgba(0.697324, 0.169573, 0.560919, 1.0) }
    ColorGradientStop { position: 0.5; color: Qt.rgba(0.798216, 0.280197, 0.469538, 1.0) }
    ColorGradientStop { position: 0.6015625; color: Qt.rgba(0.884436, 0.397139, 0.37986, 1.0) }
    ColorGradientStop { position: 0.703125; color: Qt.rgba(0.951344, 0.52285, 0.292275, 1.0) }
    ColorGradientStop { position: 0.80078125; color: Qt.rgba(0.989128, 0.658043, 0.2081, 1.0) }
    ColorGradientStop { position: 0.90234375; color: Qt.rgba(0.987621, 0.815978, 0.144363, 1.0) }
    ColorGradientStop { position: 1.0; color: Qt.rgba(0.940015, 0.975158, 0.131326, 1.0) }
    }
    property Gradient plasma_: Gradient {
    GradientStop { position: 0.0; color: Qt.rgba(0.050383, 0.029803, 0.527975, 1.0) }
    GradientStop { position: 0.1015625; color: Qt.rgba(0.261183, 0.013308, 0.617911, 1.0) }
    GradientStop { position: 0.203125; color: Qt.rgba(0.423689, 0.000646, 0.658956, 1.0) }
    GradientStop { position: 0.30078125; color: Qt.rgba(0.568201, 0.055778, 0.639477, 1.0) }
    GradientStop { position: 0.40234375; color: Qt.rgba(0.697324, 0.169573, 0.560919, 1.0) }
    GradientStop { position: 0.5; color: Qt.rgba(0.798216, 0.280197, 0.469538, 1.0) }
    GradientStop { position: 0.6015625; color: Qt.rgba(0.884436, 0.397139, 0.37986, 1.0) }
    GradientStop { position: 0.703125; color: Qt.rgba(0.951344, 0.52285, 0.292275, 1.0) }
    GradientStop { position: 0.80078125; color: Qt.rgba(0.989128, 0.658043, 0.2081, 1.0) }
    GradientStop { position: 0.90234375; color: Qt.rgba(0.987621, 0.815978, 0.144363, 1.0) }
    GradientStop { position: 1.0; color: Qt.rgba(0.940015, 0.975158, 0.131326, 1.0) }
    }
}
