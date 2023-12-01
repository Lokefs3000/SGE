#pragma once

static const char* res_shaders_debug_font_vert =
      "#version 330\n"
      "#extension GL_ARB_separate_shader_objects : enable\n"
      "layout(location = 0) in vec4 vVertex;\n"
      "layout(location = 1) in vec4 vColor;\n"
      "layout(location = 0) out vec2 fTexCoord;\n"
      "layout(location = 1) out vec4 fColor;\n"
      "uniform mat4 uProjection;\n"
      "void main() {\n"
      "    gl_Position = vec4(vVertex.xy, 0.0, 1.0) * uProjection;\n"
      "    fTexCoord = vVertex.zw;\n"
      "    fColor = vColor;\n"
      "}\n";