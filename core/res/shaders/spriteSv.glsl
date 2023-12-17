#version 330 core
layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

$out vec2 fTexCoord;
$out vec4 fColor;

uniform mat4 uProjection;

void main() {
    gl_Position = vec4(vVertex.xy, 0.0, 1.0) * uProjection;

    fTexCoord = vVertex.zw;
    fColor = vColor;
}