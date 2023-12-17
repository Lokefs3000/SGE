using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace em_builder
{
    public class Program
    {
        static void CompileAsLib(List<string> input, List<string> includes, List<string> libs, bool debug, string inter, string output, string em, string name, int size, int optimize)
        {
            string incCmd = "";

            for (int i = 0; i < includes.Count; i++)
            {
                incCmd += "-I " + Path.GetFullPath(includes[i]) + " ";
            }

            string libCmd = "";

            for (int i = 0; i < libs.Count; i++)
            {
                incCmd += Path.GetFullPath(libs[i]) + " ";
            }

            string compileCmd = "@call " + Path.Combine(Path.GetFullPath(em), "emsdk_env.bat") + "\n";

            if (debug)
            {
                compileCmd += "set EMCC_DEBUG=1\n";
            }

            List<string> objects = new List<string>();

            for (int i = 0; i < input.Count; i++)
            {
                string cmd = "@call emcc " + Path.GetFullPath(input[i]) + " " + libCmd + " -c -o " + Path.Combine(Path.GetFullPath(inter), Path.GetFileNameWithoutExtension(input[i])) + ".o -O" + optimize + " " + incCmd + "  -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2  ";
                if (debug)
                {
                    cmd += "-sDEMANGLE_SUPPORT=1 -g" + size;
                }
                objects.Add(Path.Combine(Path.GetFullPath(inter), Path.GetFileNameWithoutExtension(input[i])) + ".o");
                compileCmd += cmd + "\n";
            }

            compileCmd += "@call emar rcs " + Path.Combine(Path.GetFullPath(output), name) + ".a ";

            for (int i = 0; i < objects.Count; i++)
            {
                compileCmd += objects[i] + " ";
            }

            File.WriteAllText("temp.bat", compileCmd + "\n@pause");
            Process p = Process.Start("temp.bat");
            p.WaitForExit();

            Directory.Delete(inter, true);
            File.Delete("temp.bat");
        }

        static void CompileAsWasm(List<string> input, List<string> includes, List<string> libs, bool debug, string inter, string output, string em, string name, int size, int optimize, bool embed, List<string> resources)
        {
            string incCmd = "";

            for (int i = 0; i < includes.Count; i++)
            {
                incCmd += "-I " + Path.GetFullPath(includes[i]) + " ";
            }

            string libCmd = "";

            for (int i = 0; i < libs.Count; i++)
            {
                incCmd += Path.GetFullPath(libs[i]) + " ";
            }

            string compileCmd = "@call " + Path.Combine(Path.GetFullPath(em), "emsdk_env.bat") + "\n";

            if (debug)
            {
                compileCmd += "set EMCC_DEBUG=1\n";
            }

            compileCmd += "@call emcc -O" + optimize + " -g" + size + " -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 ";

            for (int i = 0; i < input.Count; i++)
            {
                compileCmd += Path.GetFullPath(input[i]) + " ";
            }

            compileCmd += "-o " + Path.Combine(Path.GetFullPath(output), name) + ".html " + incCmd + libCmd;

            if (embed)
            {
                for (int i = 0; i < resources.Count; i++)
                {
                    compileCmd += "--preload-file " + resources[i] + " ";
                }
            }

            File.WriteAllText("temp.bat", compileCmd + "\n@pause");
            Process p = Process.Start("temp.bat");
            p.WaitForExit();

            Directory.Delete(inter, true);
            File.Delete("temp.bat");
        }

        static void Main(string[] args)
        {
            bool isLib = false;
            bool isDebug = false;
            bool embedRes = false;

            int debugSize = 1;
            int optimize = 0;

            List<string> input = new List<string>();
            string intermediate = "";
            string output = "";

            List<string> includes = new List<string>();
            List<string> libs = new List<string>();

            List<string> resFiles = new List<string>();

            string emscriptenSource = "";
            string name = "";

            for (int i = 0; i < args.Length; i++)
            {
                switch (args[i])
                {
                    case "/lib":
                        isLib = true;
                        break;
                    case "/debug":
                        isDebug = true;
                        break;
                    case "/debsz":
                        int.TryParse(args[++i], out debugSize);
                        break;
                    case "/res":
                        embedRes = true;
                        resFiles.Add(args[++i]);
                        break;
                    case "/c":
                        input.Add(args[++i]);
                        break;
                    case "/im":
                        intermediate = args[++i];
                        break;
                    case "/o":
                        output = args[++i];
                        break;
                    case "/i":
                        includes.Add(args[++i]);
                        break;
                    case "/em":
                        emscriptenSource = args[++i];
                        break;
                    case "/name":
                        name = args[++i];
                        break;
                    case "/opt":
                        int.TryParse(args[++i], out optimize);
                        break;
                    case "/libs":
                        libs.Add(args[++i]);
                        break;
                }
            }

            if (!Directory.Exists(output))
            {
                Directory.CreateDirectory(output);
            }

            if (name.Length == 0)
            {
                name = Path.GetFileNameWithoutExtension(output);
            }

            if (intermediate.Length == 0 && isLib)
            {
                intermediate = name + "_emtemp";
            }

            if (!Directory.Exists(intermediate))
            {
                Directory.CreateDirectory(intermediate);
            }

            for (int i = 0; i < input.Count; i++)
            {
                if (!File.Exists(input[i]) && !Directory.Exists(input[i]))
                {
                    input.RemoveAt(i--);
                }
                else
                {
                    FileAttributes attr = File.GetAttributes(input[i]);

                    if (attr.HasFlag(FileAttributes.Directory))
                    {
                        string[] sub = Directory.GetFiles(input[i], "*.*", SearchOption.AllDirectories);
                        for (int j = 0; j < sub.Length; j++)
                        {
                            input.Add(sub[j]);
                        }

                        input.RemoveAt(i--);
                    }
                    else if (Path.GetExtension(input[i]) != ".cpp" && Path.GetExtension(input[i]) != ".c")
                    {
                        input.RemoveAt(i--);
                    }
                }
            }

            for (int i = 0; i < includes.Count; i++)
            {
                if (!File.Exists(includes[i]) && !Directory.Exists(includes[i]))
                {
                    includes.RemoveAt(i--);
                }
                else
                {
                    FileAttributes attr = File.GetAttributes(includes[i]);

                    if (!attr.HasFlag(FileAttributes.Directory))
                    {
                        includes.RemoveAt(i--);
                    }
                }
            }

            for (int i = 0; i < libs.Count; i++)
            {
                if (!File.Exists(libs[i]))
                {
                    libs.RemoveAt(i--);
                }
                else
                {
                    FileAttributes attr = File.GetAttributes(libs[i]);

                    if (attr.HasFlag(FileAttributes.Directory))
                    {
                        libs.RemoveAt(i--);
                    }
                    else if (Path.GetExtension(libs[i]) != ".a")
                    {
                        libs.RemoveAt(i--);
                    }
                }
            }

            if (emscriptenSource.Length == 0)
            {
                Console.Error.WriteLine("Emscripten source not specified! use \"/em {path}\" to specify!");
                return;
            }

            debugSize = Math.Min(Math.Max(debugSize, 0), 3);

            if (isDebug)
            {
                optimize = 0;
            }
            optimize = Math.Min(Math.Max(optimize, 0), 3);

            if (isLib)
            {
                CompileAsLib(input, includes, libs, isDebug, intermediate, output, emscriptenSource, name, debugSize, optimize);
            }
            else
            {
                CompileAsWasm(input, includes, libs, isDebug, intermediate, output, emscriptenSource, name, debugSize, optimize, embedRes, resFiles);
            }
        }
    }
}
