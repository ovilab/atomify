import ShaderNodes 1.0

ShaderNode {
    property var vector: ShaderBuilderBinding {
        property: "textureCoordinate"
        defaultValue: Qt.vector3d(0.0, 0.0, 0.0)
    }
    property var scale: 1.0
    property var strength: 1.0
    property var detail: 4.0
    property var offset: Qt.vector3d(0.0, 0.0, 0.0)

    exportedTypeName: "Simplex"

    type: "vec3"
    name: {
        console.log("Setting name")
        return "simplex"
    }
    result: "simplexNoise($(vector, vec3) + $(offset, vec3), $(scale, float), $(strength, float), $(detail, float))"
    header: "
// GLSL textureless classic 4D noise \"cnoise\",
// with an RSL-style periodic variant \"pnoise\".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-08-22
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashimavec/webgl-noise
//

#ifndef MOD289VEC3
#define MOD289VEC3
highp vec3 mod289(highp vec3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
#endif

#ifndef MOD289VEC4
#define MOD289VEC4
highp vec4 mod289(highp vec4 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
#endif

#ifndef PERMUTEVEC4
#define PERMUTEVEC4
highp vec4 permute(highp vec4 x)
{
    return mod289(((x*34.0)+1.0)*x);
}
#endif

#ifndef TAYLORINVSQRTVEC4
#define TAYLORINVSQRTVEC4
highp vec4 taylorInvSqrt(highp vec4 r)
{
    return 1.79284291400159 - 0.85373472095314 * r;
}
#endif

highp float snoise(highp vec3 v)
{
    const highp vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
    const highp vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

    // First corner
    highp vec3 i  = floor(v + dot(v, C.yyy) );
    highp vec3 x0 =   v - i + dot(i, C.xxx) ;

    // Other corners
    highp vec3 g = step(x0.yzx, x0.xyz);
    highp vec3 l = 1.0 - g;
    highp vec3 i1 = min( g.xyz, l.zxy );
    highp vec3 i2 = max( g.xyz, l.zxy );

    //   x0 = x0 - 0.0 + 0.0 * C.xxx;
    //   x1 = x0 - i1  + 1.0 * C.xxx;
    //   x2 = x0 - i2  + 2.0 * C.xxx;
    //   x3 = x0 - 1.0 + 3.0 * C.xxx;
    highp vec3 x1 = x0 - i1 + C.xxx;
    highp vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
    highp vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

    // Permutations
    i = mod289(i);
    highp vec4 p = permute( permute( permute(
                                   i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
                               + i.y + vec4(0.0, i1.y, i2.y, 1.0 ))
                      + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

    // Gradients: 7x7 points over a square, mapped onto an octahedron.
    // The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
    highp float n_ = 0.142857142857; // 1.0/7.0
    highp vec3  ns = n_ * D.wyz - D.xzx;

    highp vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

    highp vec4 x_ = floor(j * ns.z);
    highp vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

    highp vec4 x = x_ *ns.x + ns.yyyy;
    highp vec4 y = y_ *ns.x + ns.yyyy;
    highp vec4 h = 1.0 - abs(x) - abs(y);

    highp vec4 b0 = vec4( x.xy, y.xy );
    highp vec4 b1 = vec4( x.zw, y.zw );

    //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
    //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
    highp vec4 s0 = floor(b0)*2.0 + 1.0;
    highp vec4 s1 = floor(b1)*2.0 + 1.0;
    highp vec4 sh = -step(h, vec4(0.0));

    highp vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
    highp vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

    highp vec3 p0 = vec3(a0.xy,h.x);
    highp vec3 p1 = vec3(a0.zw,h.y);
    highp vec3 p2 = vec3(a1.xy,h.z);
    highp vec3 p3 = vec3(a1.zw,h.w);

    //Normalise gradients
    highp vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
    p0 *= norm.x;
    p1 *= norm.y;
    p2 *= norm.z;
    p3 *= norm.w;

    // Mix final noise value
    highp vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1),
                                  dot(p2,x2), dot(p3,x3) ) );
}
float simplexDetail(vec3 x, float detail) {
    const mat3 m = mat3( 0.00,  0.80,  0.60,
                        -0.80,  0.36, -0.48,
                        -0.60, -0.48,  0.64 );
    vec3 q = x;
    float f = 0.0;
    float factor = 1.0;
    for(int i = 0; i < int(detail) + 1; i++) {
        factor *= 0.5;
        f += factor*snoise( q );
        q = m*q*(2.0 + 0.01 * i);
    }
    factor *= 0.5;
    float remainder = detail - float(int(detail));
    f += remainder*factor*snoise( q );
    return f;
}
highp vec3 simplexNoise(highp vec3 vertexPosition, float scale, float intensity, float detail) {
    highp float nx = simplexDetail(0.5*scale*(vertexPosition.xyz + vertexPosition.zzz + vec3(2.0,2.0,-2.0)), detail);
    highp float ny = simplexDetail(0.5*scale*(vertexPosition.yzx + vertexPosition.xxx + vec3(-2.0,2.0,2.0)), detail);
    highp float nz = simplexDetail(0.5*scale*(vertexPosition.zyz + vertexPosition.yyy + vec3(2.0,-2.0,2.0)), detail);
    return vec3(nx, ny, nz)*intensity;
}
"
}
