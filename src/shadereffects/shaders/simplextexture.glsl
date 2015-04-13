// BEGIN simplextexture.glsl
#ifndef SIMPLEXTEXTUREGLSL
#define SIMPLEXTEXTUREGLSL
uniform highp float cp_simplexTextureScale;

lowp float surface(highp vec3 coord ) {

    highp float frequency = 4.0;
    highp float n = 0.0;

#ifdef TIMEDEPENDENTSIMPLEXTEXTURE
    highp vec4 coord2 = vec4(coord, 0.1*cp_time);
#else
    highp vec3 coord2 = coord;
#endif
    coord *= cp_simplexTextureScale;

    n += 1.0	* abs( snoise( coord2 * frequency ) );
    n += 0.5	* abs( snoise( coord2 * frequency * 2.0 ) );
    n += 0.25	* abs( snoise( coord2 * frequency * 4.0 ) );

    return n;
}

#endif
// END simplextexture.glsl
