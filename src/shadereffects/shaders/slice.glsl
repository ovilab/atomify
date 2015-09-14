// BEGIN slice.glsl
#ifndef SLICEGLSL
#define SLICEGLSL
#ifdef SLICE
    uniform vec3 sliceOrigo;
    uniform vec3 sliceNormal;
    uniform float sliceWidth;

    float distanceToPlane(vec3 position, vec3 plane, vec3 normal) {
        return dot((position - plane), normal);
    }

    bool slice_vectorIsInside(vec3 position) {
        return abs(distanceToPlane(position, sliceOrigo, sliceNormal)) < 0.5*sliceWidth;
    }

    bool slice_vectorIsInside(vec4 position) {
        return abs(distanceToPlane(position.xyz, sliceOrigo, sliceNormal)) < 0.5*sliceWidth;
    }
#endif
#endif
// END slice.glsl
