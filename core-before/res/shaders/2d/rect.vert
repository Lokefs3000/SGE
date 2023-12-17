#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 fColor;

uniform mat4 uProjection;

void main() {
    gl_Position = vec4(vVertex.xy, 0.0, 1.0) * uProjection;
    fColor = vColor;
}