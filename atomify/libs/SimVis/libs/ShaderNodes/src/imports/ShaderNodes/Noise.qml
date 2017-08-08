import ShaderNodes 1.0

ShaderNode {
    property var vector: ShaderBuilderBinding {
        property: "textureCoordinate"
    }

    property var scale: 1.0
    property var detail: 4.0

    exportedTypeName: "Noise"

    type: "vec3"
    name: "noise"
    result: "noise3($(vector, vec3), $(scale, float), $(detail, float));"
    header: "
        float hash( float n ) { return fract(sin(n)*753.5453123); }
        float noise( vec3 x)
        {
            vec3 p = floor(x);
            vec3 f = fract(x);
            f = f*f*(3.0-2.0*f);

            float n = p.x + p.y*157.0 + 113.0*p.z;
            return mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                           mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
                       mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                           mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
        }
        float noiseDetail(vec3 x, float detail) {
            const mat3 m = mat3( 0.00,  0.80,  0.60,
                                -0.80,  0.36, -0.48,
                                -0.60, -0.48,  0.64 );
            vec3 q = x;
            float f = 0.0;
            float factor = 1.0;
            for(int i = 0; i < int(detail) + 1; i++) {
                factor *= 0.5;
                f += factor*noise( q );
                q = m*q*(2.0 + 0.01 * i);
            }
            factor *= 0.5;
            float remainder = detail - float(int(detail));
            f += remainder*factor*noise( q );
            return f;
        }
        vec3 noise3(vec3 v, float scale, float detail) {
            highp float nx = noiseDetail(v.xyz * scale, detail);
            highp float ny = noiseDetail(v.yzx * scale, detail);
            highp float nz = noiseDetail(v.zyx * scale, detail);
            return vec3(nx, ny, nz);
        }
    "
}
