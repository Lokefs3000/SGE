::@echo Compiling debug glad..
::@call util\wasm\em-builder.exe /lib /debug /debsz 3 /c glad/ /im web/temp/Debug /o web/Debug/ /i include/ /name gladd /em ../../tools/emscripten/emsdk/
@echo Compiling debug core..
@call util\wasm\em-builder.exe /lib /debug /debsz 3 /c core/ /im web/temp/Debug /o web/Debug/ /i include/ /name sge1d /em ../../tools/emscripten/emsdk/
@echo Compiling debug app..
@call util\wasm\em-builder.exe /debug /debsz 3 /c appc/ /im web/temp/Debug /o web/Debug/ /i core/ /name appc /em ../../tools/emscripten/emsdk/ /libs web/Debug/sge1d.a /libs web/Lib/libSDL3.a /libs web/Debug/gladd.a /libs web/Lib/libLua52.a /res web/data.pak@data.pak /libs web/Lib/libz.a