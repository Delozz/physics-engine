#version 330 core

layout(location = 0) in vec3 aPosition; // unit sphere, also the object-space normal
layout(location = 1) in mat4 aModel;    // locations 1-4
layout(location = 5) in vec3 aColor;

uniform mat4 uViewProj;

out vec3 vWorldPos;
out vec3 vWorldNormal;
out vec3 vObjectDir;
out vec3 vColor;

void main() {
    vec4 world = aModel * vec4(aPosition, 1.0);
    vWorldPos = world.xyz;
    // Uniform scale only, so the upper 3x3 of the model matrix is fine for normals.
    vWorldNormal = mat3(aModel) * aPosition;
    vObjectDir = aPosition;
    vColor = aColor;
    gl_Position = uViewProj * world;
}
