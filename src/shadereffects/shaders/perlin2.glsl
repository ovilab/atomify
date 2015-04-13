// BEGIN perlin2.glsl
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
#ifndef PERLIN2GLSL
#define PERLIN2GLSL

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

#ifndef FADEVEC2
#define FADEVEC2
highp vec2 fade(highp vec2 t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}
#endif

// Classic Perlin noise
highp float cnoise(highp vec2 P)
{
  highp vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  highp vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod289(Pi); // To avoid truncation effects in permutation
  highp vec4 ix = Pi.xzxz;
  highp vec4 iy = Pi.yyww;
  highp vec4 fx = Pf.xzxz;
  highp vec4 fy = Pf.yyww;

  highp vec4 i = permute(permute(ix) + iy);

  highp vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  highp vec4 gy = abs(gx) - 0.5 ;
  highp vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  highp vec2 g00 = vec2(gx.x,gy.x);
  highp vec2 g10 = vec2(gx.y,gy.y);
  highp vec2 g01 = vec2(gx.z,gy.z);
  highp vec2 g11 = vec2(gx.w,gy.w);

  highp vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;

  highp float n00 = dot(g00, vec2(fx.x, fy.x));
  highp float n10 = dot(g10, vec2(fx.y, fy.y));
  highp float n01 = dot(g01, vec2(fx.z, fy.z));
  highp float n11 = dot(g11, vec2(fx.w, fy.w));

  highp vec2 fade_xy = fade(Pf.xy);
  highp vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  highp float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}

// Classic Perlin noise, periodic variant
highp float pnoise(highp vec2 P, highp vec2 rep)
{
  highp vec4 Pi = floor(P.xyxy) + vec4(0.0, 0.0, 1.0, 1.0);
  highp vec4 Pf = fract(P.xyxy) - vec4(0.0, 0.0, 1.0, 1.0);
  Pi = mod(Pi, rep.xyxy); // To create noise with explicit period
  Pi = mod289(Pi);        // To avoid truncation effects in permutation
  highp vec4 ix = Pi.xzxz;
  highp vec4 iy = Pi.yyww;
  highp vec4 fx = Pf.xzxz;
  highp vec4 fy = Pf.yyww;

  highp vec4 i = permute(permute(ix) + iy);

  highp vec4 gx = fract(i * (1.0 / 41.0)) * 2.0 - 1.0 ;
  highp vec4 gy = abs(gx) - 0.5 ;
  highp vec4 tx = floor(gx + 0.5);
  gx = gx - tx;

  highp vec2 g00 = vec2(gx.x,gy.x);
  highp vec2 g10 = vec2(gx.y,gy.y);
  highp vec2 g01 = vec2(gx.z,gy.z);
  highp vec2 g11 = vec2(gx.w,gy.w);

  highp vec4 norm = taylorInvSqrt(vec4(dot(g00, g00), dot(g01, g01), dot(g10, g10), dot(g11, g11)));
  g00 *= norm.x;
  g01 *= norm.y;
  g10 *= norm.z;
  g11 *= norm.w;

  highp float n00 = dot(g00, vec2(fx.x, fy.x));
  highp float n10 = dot(g10, vec2(fx.y, fy.y));
  highp float n01 = dot(g01, vec2(fx.z, fy.z));
  highp float n11 = dot(g11, vec2(fx.w, fy.w));

  highp vec2 fade_xy = fade(Pf.xy);
  highp vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade_xy.x);
  highp float n_xy = mix(n_x.x, n_x.y, fade_xy.y);
  return 2.3 * n_xy;
}
#endif
// END perlin2.glsl
