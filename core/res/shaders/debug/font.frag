#version 330 core
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec2 fTexCoord;
layout(location = 1) in vec4 fColor;

layout(location = 0) out vec4 fFrag;

uniform sampler2D uFont;

void main() {
    float alpha = texture2D(uFont, fTexCoord).r;
    fFrag = vec4(1.0, 1.0, 1.0, alpha) * fColor;
}