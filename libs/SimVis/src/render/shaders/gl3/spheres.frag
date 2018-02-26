#version 330
#pragma shadernodes header

in vec3 modelPosition;
in vec3 modelSpherePosition;
in vec3 color;
in vec2 planePosition;
in float vs_particleId;
in float vs_flags;
in float radius;

out float particleIdOut;
out vec4 colorOut;

uniform mat4 modelView;
uniform mat4 inverseModelView;
uniform mat4 inverseViewMatrix;
uniform vec3 eyePosition;
uniform vec3 viewVector;

void main(void) {
    vec3 rayDirection = eyePosition - modelPosition;
    vec3 rayOrigin = modelPosition - modelSpherePosition;

    vec3 E = rayOrigin;
    vec3 D = rayDirection;

    // Sphere equation
    //      x^2 + y^2 + z^2 = r^2
    // Ray equation is
    //     P(t) = E + t*D
    // We substitute ray into sphere equation to get
    //     (Ex + Dx * t)^2 + (Ey + Dy * t)^2 + (Ez + Dz * t)^2 = r^2
    float r2 = radius*radius;

    // float a = D.x*D.x + D.y*D.y + D.z*D.z;
    // float b = 2.0*E.x*D.x + 2.0*E.y*D.y + 2.0*E.z*D.z;
    // float c = E.x*E.x + E.y*E.y + E.z*E.z - r2;

    float a = dot(D,D);
    float bHalf = dot(E,D); // We don't need the factor 2
    float c = dot(E,E) - r2;

    // discriminant of sphere equation
    // float d = b*b - 4.0*a*c;
    float d = bHalf*bHalf - a*c; // Factor 4 is cancelled because of bHalf and /(2.0*a) later.
    if(d < 0.0) {
        discard;
    }

    // float t = (-b + sqrt(d))/(2.0*a);
    float t = (-bHalf + sqrt(d))/a;
    vec3 sphereIntersection = rayOrigin + t * rayDirection;

    vec3 normal = normalize(sphereIntersection);
    vec3 normalDotCamera = color*dot(normal, normalize(rayDirection));

    float pi = 3.1415926535897932384626433832795;

    vec3 position = modelSpherePosition + sphereIntersection;

#pragma shadernodes body

    if (vs_flags > 0.5) {
        colorOut.r *= 2;
    }
    particleIdOut = vs_particleId;
}
