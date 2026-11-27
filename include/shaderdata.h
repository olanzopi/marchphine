//not my shader!!!! this is a placeholden, WILL be changed.

#pragma once

inline const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 normal;

out vec3 vRayOrigin;
out vec3 vRayTarget;

uniform mat4 MVP;
uniform mat4 Model;

void main()
{
    gl_Position = vec4(aPos.xy, 0.0, 1.0);

    // Keep Model active so getUniformLocation("Model") never fails
    mat4 invMVP = inverse(MVP * mat4(mat3(Model) * 0.0 + mat3(1.0)));

    vec4 nearPoint = invMVP * vec4(aPos.xy, -1.0, 1.0);
    vec4 farPoint  = invMVP * vec4(aPos.xy,  1.0, 1.0);

    vRayOrigin = nearPoint.xyz / nearPoint.w;
    vRayTarget = farPoint.xyz / farPoint.w;
}
)";

inline const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 vRayOrigin;
in vec3 vRayTarget;

const int MAX_STEPS = 100;
const float MAX_DIST = 100.0;
const float SURF_DIST = 0.001;

// --- Primitive SDFs ---
float sdSphere(vec3 p, float r) {
    return length(p) - r;
}

float sdRoundBox(vec3 p, vec3 b, float r) {
    vec3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdTorus(vec3 p, vec2 t) {
    vec2 q = vec2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

// Smooth minimum for organic blending
float smin(float a, float b, float k) {
    float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
    return mix(b, a, h) - k * h * (1.0 - h);
}

// Returns vec2(distance, materialID)
vec2 getDist(vec3 p) {
    float floorDist = p.y + 1.0;

    // Smoothly blended sphere + rounded box
    float sphere = sdSphere(p - vec3(-1.2, 0.0, 4.5), 0.9);
    float box    = sdRoundBox(p - vec3(0.3, -0.1, 4.5), vec3(0.75), 0.12);
    float blend  = smin(sphere, box, 0.45);

    // Floating torus on the right
    float torus  = sdTorus(p - vec3(2.4, 0.2, 4.5), vec2(0.75, 0.25));

    vec2 res = vec2(floorDist, 1.0);
    if (blend < res.x) res = vec2(blend, 2.0);
    if (torus < res.x) res = vec2(torus, 3.0);

    return res;
}

// Sphere tracing
vec2 rayMarch(vec3 ro, vec3 rd) {
    float dO = 0.0;
    float mat = 0.0;

    for (int i = 0; i < MAX_STEPS; i++) {
        vec3 p = ro + rd * dO;
        vec2 res = getDist(p);
        dO += res.x;
        mat = res.y;

        if (dO > MAX_DIST || abs(res.x) < SURF_DIST)
            break;
    }
    return vec2(dO, mat);
}

// Tetrahedron normal
vec3 getNormal(vec3 p) {
    const float h = 0.0005;
    const vec2 k = vec2(1.0, -1.0);
    return normalize(
        k.xyy * getDist(p + k.xyy * h).x +
        k.yyx * getDist(p + k.yyx * h).x +
        k.yxy * getDist(p + k.yxy * h).x +
        k.xxx * getDist(p + k.xxx * h).x
    );
}

// Soft shadow raymarch
float getShadow(vec3 ro, vec3 rd, float maxD) {
    float res = 1.0;
    float t = 0.02;
    for (int i = 0; i < 40 && t < maxD; i++) {
        float h = getDist(ro + rd * t).x;
        res = min(res, 12.0 * h / t);
        t += clamp(h, 0.02, 0.25);
        if (res < 0.001) break;
    }
    return clamp(res, 0.0, 1.0);
}

void main() {
    vec3 ro = vRayOrigin;
    vec3 rd = normalize(vRayTarget - vRayOrigin);

    vec2 hit = rayMarch(ro, rd);
    float d = hit.x;
    float mat = hit.y;

    // Sky background
    vec3 col = mix(vec3(0.45, 0.65, 0.90), vec3(0.12, 0.22, 0.42), clamp(rd.y * 0.8 + 0.2, 0.0, 1.0));

    if (d < MAX_DIST) {
        vec3 p = ro + rd * d;
        vec3 n = getNormal(p);

        vec3 lightPos = vec3(3.0, 6.0, 1.0);
        vec3 l = normalize(lightPos - p);
        float diff = clamp(dot(n, l), 0.0, 1.0);
        float sh = getShadow(p + n * 0.01, l, length(lightPos - p));

        // Material colors
        vec3 baseCol = vec3(0.8);
        if (mat == 1.0) {
            float checker = mod(floor(p.x) + floor(p.z), 2.0);
            baseCol = mix(vec3(0.25, 0.28, 0.32), vec3(0.55, 0.58, 0.62), checker);
        } else if (mat == 2.0) {
            baseCol = vec3(0.25, 0.65, 0.95); // Blue blended sphere + box
        } else if (mat == 3.0) {
            baseCol = vec3(0.95, 0.45, 0.25); // Orange torus
        }

        // Specular highlight
        vec3 halfDir = normalize(l - rd);
        float spec = pow(max(dot(n, halfDir), 0.0), 32.0);

        col = baseCol * (0.2 + 0.8 * diff * sh) + vec3(0.4) * spec * sh;

        // Distance fog
        col = mix(col, vec3(0.45, 0.65, 0.90), 1.0 - exp(-0.0008 * d * d));
    }

    col = pow(col, vec3(0.4545)); // Gamma correction
    FragColor = vec4(col, 1.0);
}
)";
