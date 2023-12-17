#pragma once

static const char* res_shaders_spriteSf_glsl =
      "#version 330 core\n"
      "$in vec2 fTexCoord;\n"
      "$in vec4 fColor;\n"
      "$out vec4 fFragColor;\n"
      "uniform sampler2D uTexture;\n"
      "void main() {\n"
      "    fFragColor = texture(uTexture, fTexCoord) * fColor;\n"
      "}\n";