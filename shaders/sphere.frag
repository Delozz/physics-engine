#version 330 core

in vec3 vWorldPos;
in vec3 vWorldNormal;
in vec3 vObjectDir;
in vec3 vColor;

uniform vec3 uCameraPos;
uniform vec3 uLightDir; // toward the light

out vec4 fragColor;

const float PI = 3.14159265;

void main() {
    // 8 x 4 checker in longitude/latitude of the object-space direction, so it rotates with
    // the body and spin is visible.
    vec3 d = normalize(vObjectDir);
    float lon = atan(d.z, d.x);
    float lat = asin(clamp(d.y, -1.0, 1.0));
    int u = int(floor((lon + PI) / (2.0 * PI) * 8.0));
    int v = int(floor((lat + 0.5 * PI) / PI * 4.0));
    vec3 base = ((u + v) % 2 == 0) ? vColor : vColor * 0.45;

    vec3 n = normalize(vWorldNormal);
    vec3 l = normalize(uLightDir);
    vec3 h = normalize(l + normalize(uCameraPos - vWorldPos));
    float diffuse = max(dot(n, l), 0.0);
    float specular = pow(max(dot(n, h), 0.0), 48.0);

    vec3 color = base * (0.2 + 0.8 * diffuse) + vec3(0.25) * specular;
    fragColor = vec4(color, 1.0);
}
