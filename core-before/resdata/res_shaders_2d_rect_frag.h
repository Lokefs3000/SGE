#pragma once

static const char* res_shaders_2d_rect_frag =
      "#version 330 core\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "layout(location = 0) in vec4 fColor;\n"
      "layout(location = 0) out vec4 fFrag;\n"
      "void main() {\n"
      "    fFrag = fColor;\n"
      "}\n";