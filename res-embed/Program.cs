using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace res_embed
{
    internal class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Beginning embed..");

            string dir = args[0];
            string outdir = args[1];

            string output = "";

            string[] files = Directory.GetFiles(dir, "*.*", SearchOption.AllDirectories);
            for (int i = 0; i < files.Length; i++)
            {
                string data = LoadFile(files[i]);
                output = "#pragma once\n\nstatic const char* res" + files[i].Replace('\\', '_').Replace('.', '_').Substring(dir.Length) + " =\n" + data;
                File.WriteAllText(outdir + "\\res" + files[i].Replace('\\', '_').Replace('.', '_').Substring(dir.Length) + ".h", output);
            }

            Console.WriteLine("Embed completed!");
        }

        private static string LoadFile(string path)
        {
            string data = "";
            string[] lines = File.ReadAllLines(path);

            for (int i = 0; i < lines.Length; i++)
            {
                if (lines[i].Length > 0)
                {
                    data += "      " + '"' + lines[i].Replace("\"", "\\\"") + "\\n" + '"' + "\n";
                }
            }

            if (data.Length > 0)
            {
                data = data.Substring(0, data.Length - 1) + ";";
            }

            return data;
        }
    }
}
