// BEGIN perlin3.glsl
//
// GLSL textureless classic 3D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
// Author:  Stefan Gustavson (stefan.gustavson@liu.se)
// Version: 2011-10-11
//
// Many thanks to Ian McEwan of Ashima Arts for the
// ideas for permutation and gradient selection.
//
// Copyright (c) 2011 Stefan Gustavson. All rights reserved.
// Distributed under the MIT license. See LICENSE file.
// https://github.com/ashima/webgl-noise
//
#ifndef PERLIN3GLSL
#define PERLIN3GLSL

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

#ifndef FADEVEC3
#define FADEVEC3
highp vec3 fade(highp vec3 t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0);
}
#endif

// Classic Perlin noise
highp float cnoise(highp vec3 P)
{
    highp vec3 Pi0 = floor(P); // Integer part for indexing
    highp vec3 Pi1 = Pi0 + vec3(1.0); // Integer part + 1
    Pi0 = mod289(Pi0);
    Pi1 = mod289(Pi1);
    highp vec3 Pf0 = fract(P); // Fractional part for interpolation
    highp vec3 Pf1 = Pf0 - vec3(1.0); // Fractional part - 1.0
    highp vec4 ix = vec4(Pi0.x, Pi1.x, Pi0.x, Pi1.x);
    highp vec4 iy = vec4(Pi0.yy, Pi1.yy);
    highp vec4 iz0 = Pi0.zzzz;
    highp vec4 iz1 = Pi1.zzzz;

    highp vec4 ixy = permute(permute(ix) + iy);
    highp vec4 ixy0 = permute(ixy + iz0);
    highp vec4 ixy1 = permute(ixy + iz1);

    highp vec4 gx0 = ixy0 * (1.0 / 7.0);
    highp vec4 gy0 = fract(floor(gx0) * (1.0 / 7.0)) - 0.5;
    gx0 = fract(gx0);
    highp vec4 gz0 = vec4(0.5) - abs(gx0) - abs(gy0);
    highp vec4 sz0 = step(gz0, vec4(0.0));
    gx0 -= sz0 * (step(0.0, gx0) - 0.5);
    gy0 -= sz0 * (step(0.0, gy0) - 0.5);

    highp vec4 gx1 = ixy1 * (1.0 / 7.0);
    highp vec4 gy1 = fract(floor(gx1) * (1.0 / 7.0)) - 0.5;
    gx1 = fract(gx1);
    highp vec4 gz1 = vec4(0.5) - abs(gx1) - abs(gy1);
    highp vec4 sz1 = step(gz1, vec4(0.0));
    gx1 -= sz1 * (step(0.0, gx1) - 0.5);
    gy1 -= sz1 * (step(0.0, gy1) - 0.5);

    highp vec3 g000 = vec3(gx0.x,gy0.x,gz0.x);
    highp vec3 g100 = vec3(gx0.y,gy0.y,gz0.y);
    highp vec3 g010 = vec3(gx0.z,gy0.z,gz0.z);
    highp vec3 g110 = vec3(gx0.w,gy0.w,gz0.w);
    highp vec3 g001 = vec3(gx1.x,gy1.x,gz1.x);
    highp vec3 g101 = vec3(gx1.y,gy1.y,gz1.y);
    highp vec3 g011 = vec3(gx1.z,gy1.z,gz1.z);
    highp vec3 g111 = vec3(gx1.w,gy1.w,gz1.w);

    highp vec4 norm0 = taylorInvSqrt(vec4(dot(g000, g000), dot(g010, g010), dot(g100, g100), dot(g110, g110)));
    g000 *= norm0.x;
    g010 *= norm0.y;
    g100 *= norm0.z;
    g110 *= norm0.w;
    highp vec4 norm1 = taylorInvSqrt(vec4(dot(g001, g001), dot(g011, g011), dot(g101, g101), dot(g111, g111)));
    g001 *= norm1.x;
    g011 *= norm1.y;
    g101 *= norm1.z;
    g111 *= norm1.w;

    highp float n000 = dot(g000, Pf0);
    highp float n100 = dot(g100, vec3(Pf1.x, Pf0.yz));
    highp float n010 = dot(g010, vec3(Pf0.x, Pf1.y, Pf0.z));
    highp float n110 = dot(g110, vec3(Pf1.xy, Pf0.z));
    highp float n001 = dot(g001, vec3(Pf0.xy, Pf1.z));
    highp float n101 = dot(g101, vec3(Pf1.x, Pf0.y, Pf1.z));
    highp float n011 = dot(g011, vec3(Pf0.x, Pf1.yz));
    highp float n111 = dot(g111, Pf1);

    highp vec3 fade_xyz = fade(Pf0);
    highp vec4 n_z = mix(vec4(n000, n100, n010, n110), vec4(n001, n101, n011, n111), fade_xyz.z);
    highp vec2 n_yz = mix(n_z.xy, n_z.zw, fade_xyz.y);
    highp float n_xyz = mix(n_yz.x, n_yz.y, fade_xyz.x);
    return 2.2 * n_xyz;
}
#endif
// END perlin2.glsl
