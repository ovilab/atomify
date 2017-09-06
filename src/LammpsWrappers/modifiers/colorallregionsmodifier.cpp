#include "colorallregionsmodifier.h"
#include "../regions.h"
#include "../system.h"
#include "../atomdata.h"

ColorAllRegionsModifier::ColorAllRegionsModifier()
{
    m_colors = { QVector3D(0.90196078 , 0.09803922, 0.29411765),
    QVector3D(0.23529412 , 0.70588235, 0.29411765),
    QVector3D(1.00000000 , 0.88235294, 0.09803922),
    QVector3D(0.00000000 , 0.50980392, 0.78431373),
    QVector3D(0.96078431 , 0.50980392, 0.18823529),
    QVector3D(0.56862745 , 0.11764706, 0.70588235),
    QVector3D(0.27450980 , 0.94117647, 0.94117647),
    QVector3D(0.94117647 , 0.19607843, 0.90196078),
    QVector3D(0.82352941 , 0.96078431, 0.23529412),
    QVector3D(0.98039216 , 0.74509804, 0.74509804),
    QVector3D(0.00000000 , 0.50196078, 0.50196078),
    QVector3D(0.90196078 , 0.74509804, 1.00000000),
    QVector3D(0.66666667 , 0.43137255, 0.15686275),
    QVector3D(1.00000000 , 0.98039216, 0.78431373),
    QVector3D(0.50196078 , 0.00000000, 0.00000000),
    QVector3D(0.66666667 , 1.00000000, 0.76470588),
    QVector3D(0.50196078 , 0.50196078, 0.00000000),
    QVector3D(1.00000000 , 0.84313725, 0.70588235),
    QVector3D(0.00000000 , 0.00000000, 0.50196078),
    QVector3D(0.50196078 , 0.50196078, 0.50196078),
    QVector3D(0.45098039, 0.05882353, 0.00000000),
    QVector3D(1.00000000, 0.56862745, 0.50196078),
    QVector3D(0.94901961, 0.74509804, 0.71372549),
    QVector3D(0.54901961, 0.43137255, 0.41176471),
    QVector3D(0.20000000, 0.09019608, 0.05098039),
    QVector3D(1.00000000, 0.54901961, 0.25098039),
    QVector3D(0.60000000, 0.32941176, 0.14901961),
    QVector3D(0.65098039, 0.57254902, 0.48627451),
    QVector3D(0.80000000, 0.53333333, 0.00000000),
    QVector3D(0.40000000, 0.26666667, 0.00000000),
    QVector3D(0.20000000, 0.16078431, 0.00000000),
    QVector3D(0.94901961, 0.88627451, 0.00000000),
    QVector3D(0.72549020, 0.74901961, 0.37647059),
    QVector3D(0.33333333, 0.34901961, 0.2627451 ),
    QVector3D(0.26274510, 0.34901961, 0.08627451),
    QVector3D(0.12549020, 0.94901961, 0.00000000),
    QVector3D(0.14901961, 0.60000000, 0.14901961),
    QVector3D(0.00000000, 0.20000000, 0.07843137),
    QVector3D(0.50196078, 1.00000000, 0.76470588),
    QVector3D(0.27450980, 0.54901961, 0.42352941),
    QVector3D(0.74901961, 1.00000000, 0.94901961),
    QVector3D(0.00000000, 0.93333333, 1.00000000),
    QVector3D(0.08627451, 0.33333333, 0.34901961),
    QVector3D(0.18823529, 0.23921569, 0.25098039),
    QVector3D(0.21176471, 0.63921569, 0.85098039),
    QVector3D(0.10196078, 0.25882353, 0.40000000),
    QVector3D(0.56078431, 0.66274510, 0.74901961),
    QVector3D(0.00000000, 0.16078431, 0.40000000),
    QVector3D(0.45098039, 0.45098039, 0.90196078),
    QVector3D(0.65490196, 0.60000000, 0.80000000),
    QVector3D(0.53333333, 0.00000000, 1.00000000),
    QVector3D(0.35686275, 0.13725490, 0.54901961),
    QVector3D(0.35294118, 0.30196078, 0.40000000),
    QVector3D(0.23921569, 0.00000000, 0.30196078),
    QVector3D(1.00000000, 0.00000000, 0.93333333),
    QVector3D(0.70196078, 0.34901961, 0.63137255),
    QVector3D(1.00000000, 0.74901961, 0.91764706),
    QVector3D(1.00000000, 0.25098039, 0.65098039),
    QVector3D(0.40000000, 0.10196078, 0.18039216),
    QVector3D(0.85098039, 0.42352941, 0.53725490),
    QVector3D(0.85098039, 0.00000000, 0.11372549) };
}

void ColorAllRegionsModifier::apply(AtomData &atomData)
{
    if(!enabled()) return;

    QList<CPRegion*> regions = m_system->regions()->regions();
    for(int i=0; i<atomData.size(); i++) {
        for(int j=0; j<regions.size(); j++) {
            CPRegion* region = regions[j];
            bool isInsideRegion = region->containsAtom(i); // Each group in LAMMPS is represented as a bit in an int. Bitwise or to check membership.
            if(isInsideRegion) {
                int colorIndex = j % m_colors.size(); // in case users have ridiculously many regions
                atomData.colors[i] = m_colors[colorIndex];
            }
        }
    }
}
