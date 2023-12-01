#pragma once

static const char* res_lua_boot_lua =
      "print(sge.def)\n"
      "package.path = sge.def .. '?.lua'\n";