// BEGIN simplex2.glsl
//
// GLSL textureless classic 4D noise "cnoise",
// with an RSL-style periodic variant "pnoise".
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

#ifndef SIMPLEX2GLSL
#define SIMPLEX2GLSL

#ifndef MOD289VEC3
#define MOD289VEC3
highp vec3 mod289(highp vec3 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
#endif

#ifndef MOD289VEC2
#define MOD289VEC2
highp vec2 mod289(highp vec2 x)
{
    return x - floor(x * (1.0 / 289.0)) * 289.0;
}
#endif

#ifndef PERMUTEVEC3
#define PERMUTEVEC3
highp vec3 permute(highp vec3 x)
{
    return mod289(((x*34.0)+1.0)*x);
}
#endif

highp float snoise(highp vec2 v)
  {
  const highp vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  highp vec2 i  = floor(v + dot(v, C.yy) );
  highp vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  highp vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  highp vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  highp vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
                + i.x + vec3(0.0, i1.x, 1.0 ));

  highp vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  highp vec3 x = 2.0 * fract(p * C.www) - 1.0;
  highp vec3 h = abs(x) - 0.5;
  highp vec3 ox = floor(x + 0.5);
  highp vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  highp vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}
#endif
// END simplex2.glsl
