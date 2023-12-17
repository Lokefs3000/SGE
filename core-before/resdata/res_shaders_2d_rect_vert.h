#pragma once

static const char* res_shaders_2d_rect_vert =
      "#version 330 core\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "layout(location = 0) in vec4 vVertex;\n"
      "layout(location = 1) in vec4 vColor;\n"
      "layout(location = 0) out vec4 fColor;\n"
      "uniform mat4 uProjection;\n"
      "void main() {\n"
      "    gl_Position = vec4(vVertex.xy, 0.0, 1.0) * uProjection;\n"
      "    fColor = vColor;\n"
      "}\n";