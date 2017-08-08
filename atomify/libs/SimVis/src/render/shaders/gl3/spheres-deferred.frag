#version 330 core
#pragma shadernodes header

in vec3 modelPosition;
in vec3 modelSpherePosition;
in vec3 color;
in vec2 planePosition;
in float radius;

out vec4 normalOut;
out vec4 positionOut;
out vec4 colorOut;

uniform mat4 mvp;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelView;
uniform mat4 inverseModelView;
uniform mat4 inverseViewMatrix;
uniform vec3 eyePosition;
uniform vec3 viewVector;
uniform float posMin;
uniform float posMax;

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
    float a = D.x*D.x + D.y*D.y + D.z*D.z;
    float b = 2.0*E.x*D.x + 2.0*E.y*D.y + 2.0*E.z*D.z;
    float c = E.x*E.x + E.y*E.y + E.z*E.z - r2;

    // discriminant of sphere equation
    float d = b*b - 4.0*a*c;
    if(d < 0.0) {
        discard;
    }

    float t = (-b + sqrt(d))/(2.0*a);
    vec3 sphereIntersection = rayOrigin + t * rayDirection;

    vec3 normal = normalize(sphereIntersection);
    float pi = 3.1415926535897932384626433832795;

    normalOut = vec4((normal + 1.0) / 2.0, 1.0);
    vec3 position = modelSpherePosition + sphereIntersection;
    float deltaMaxMin = posMax - posMin;

//    vec4 pv_pos = projectionMatrix * viewMatrix * vec4(position, 1.0);
//    float depthValue = pv_pos.z / pv_pos.w; // Maybe send this as alpha in position?

    positionOut = vec4((position-eyePosition-posMin) / deltaMaxMin, 1.0); // TODO fix with actual system size
    colorOut = vec4(color, 1.0);
}
