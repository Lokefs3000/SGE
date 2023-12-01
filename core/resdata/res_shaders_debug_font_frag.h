#pragma once

static const char* res_shaders_debug_font_frag =
      "#version 330\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "layout(location = 0) in vec2 fTexCoord;\n"
      "layout(location = 1) in vec4 fColor;\n"
      "layout(location = 0) out vec4 fFrag;\n"
      "uniform sampler2D uFont;\n"
      "void main() {\n"
      "    float alpha = texture2D(uFont, fTexCoord).r;\n"
      "    fFrag = vec4(1.0, 1.0, 1.0, alpha) * fColor;\n"
      "}\n";