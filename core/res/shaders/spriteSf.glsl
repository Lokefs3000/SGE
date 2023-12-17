#version 330 core
$in vec2 fTexCoord;
$in vec4 fColor;

$out vec4 fFragColor;

uniform sampler2D uTexture;

void main() {
    fFragColor = texture(uTexture, fTexCoord) * fColor;
}