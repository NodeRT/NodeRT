// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Runtime.InteropServices;

namespace NodeRTLib
{
    // Provides helper methods for compiling the genrated projects and copying the needed files for a package to an output directory
    public class NodeRTProjectBuildUtils
    {
        [Flags]
        public enum Platforms
        {
            x64 = 0x1,
            Win32 = 0x2,
        }

        private static readonly string VS_2012_CMD = "\"" + GetProgramFilesX86Dir() + "\\Microsoft Visual Studio 11.0\\Common7\\Tools\\VsDevCmd.bat\"";
        private static readonly string VS_2013_CMD = "\"" + GetProgramFilesX86Dir() + "\\Microsoft Visual Studio 12.0\\Common7\\Tools\\VsDevCmd.bat\"";

        private const string MSBUILD_CMD_TEMPLATE = "\"{0} & msbuild {1} /t:Rebuild /p:Configuration=Release /p:Platform={2}\"";

        // Builds the given project/sln for the given platforms and copies the output & package file to the output directory
        public static void BuildAndCopyToOutputFolder(string slnPath, VsVersions vsVersion, string outDir, Platforms plats, bool isGenerateDef)
        {
            if ((plats & Platforms.x64) != 0)
                BuildSln(slnPath, vsVersion, Platforms.x64);

            if ((plats & Platforms.Win32) != 0)
                BuildSln(slnPath, vsVersion, Platforms.Win32);

            if (!Directory.Exists(outDir))
                Directory.CreateDirectory(outDir);

            string packageDir = Path.GetDirectoryName(slnPath);
            
            CopyFile("package.json", packageDir, outDir);
            CopyFile("README.md", packageDir, outDir);
            CopyFile(".npmignore", packageDir, outDir);
            CopyDir("bin", packageDir, outDir);
            CopyDir("lib", packageDir, outDir);
        }

        // http://stackoverflow.com/questions/336633/how-to-detect-windows-64-bit-platform-with-net
        public static bool IsRunningOn64Bit
        {
            get
            {
                return (IntPtr.Size == 8) || InternalCheckIsWow64();
            }
        }


        [DllImport("kernel32.dll", SetLastError = true, CallingConvention = CallingConvention.Winapi)]
        [return: MarshalAs(UnmanagedType.Bool)]
        private static extern bool IsWow64Process(
            [In] IntPtr hProcess,
            [Out] out bool wow64Process
        );

        public static bool InternalCheckIsWow64()
        {
            if ((Environment.OSVersion.Version.Major == 5 && Environment.OSVersion.Version.Minor >= 1) ||
                Environment.OSVersion.Version.Major >= 6)
            {
                using (Process p = Process.GetCurrentProcess())
                {
                    bool retVal;
                    if (!IsWow64Process(p.Handle, out retVal))
                    {
                        return false;
                    }
                    return retVal;
                }
            }
            else
            {
                return false;
            }
        }

        public static string GetProgramFilesX86Dir()
        {
            if (IsRunningOn64Bit)
                return "%ProgramFiles(x86)%";
            else
                return "%ProgramFiles%";
        }

        private static string CreateBuildCmd(string slnPath, VsVersions vsVersion, Platforms platform)
        {
            if (vsVersion == VsVersions.Vs2012)
            {
                return String.Format(MSBUILD_CMD_TEMPLATE, VS_2012_CMD, slnPath, platform.ToString());
            }
            else
            {
                return String.Format(MSBUILD_CMD_TEMPLATE, VS_2013_CMD, slnPath, platform.ToString());
            }
        }

        private static void BuildSln(string slnPath, VsVersions vsVersion, Platforms platform)
        {
            string cmd = CreateBuildCmd(slnPath, vsVersion, platform);
            bool result = ExecuteCommand(cmd);

            if (!result)
                throw new Exception("Failed to build project");
        }

        private static bool ExecuteCommand(string cmd)
        {
            ProcessStartInfo ProcessInfo = new ProcessStartInfo("cmd.exe", "/c " + cmd);
            ProcessInfo.CreateNoWindow = true;
            ProcessInfo.UseShellExecute = false;
            ProcessInfo.LoadUserProfile = true;

            if (ProcessInfo.EnvironmentVariables.ContainsKey("VisualStudioDir"))
                ProcessInfo.EnvironmentVariables.Remove("VisualStudioDir");
            if (ProcessInfo.EnvironmentVariables.ContainsKey("VisualStudioEdition"))
                ProcessInfo.EnvironmentVariables.Remove("VisualStudioEdition");
            if (ProcessInfo.EnvironmentVariables.ContainsKey("VisualStudioVersion"))
                ProcessInfo.EnvironmentVariables.Remove("VisualStudioVersion");
            
            Process process;

            process = Process.Start(ProcessInfo);
            
            process.WaitForExit();
            bool success = (process.ExitCode == 0);

            process.Close();

            return success;
        }

        // http://msdn.microsoft.com/en-us/library/bb762914(v=vs.110).aspx?cs-save-lang=1&cs-lang=csharp#code-snippet-1
        private static void DirectoryCopy(string sourceDirName, string destDirName, bool copySubDirs)
        {
            // Get the subdirectories for the specified directory.
            DirectoryInfo dir = new DirectoryInfo(sourceDirName);
            DirectoryInfo[] dirs = dir.GetDirectories();

            if (!dir.Exists)
            {
                throw new DirectoryNotFoundException(
                    "Source directory does not exist or could not be found: "
                    + sourceDirName);
            }

            // If the destination directory doesn't exist, create it. 
            if (!Directory.Exists(destDirName))
            {
                Directory.CreateDirectory(destDirName);
            }

            // Get the files in the directory and copy them to the new location.
            FileInfo[] files = dir.GetFiles();
            foreach (FileInfo file in files)
            {
                string temppath = Path.Combine(destDirName, file.Name);
                file.CopyTo(temppath, true);
            }

            // If copying subdirectories, copy them and their contents to new location. 
            if (copySubDirs)
            {
                foreach (DirectoryInfo subdir in dirs)
                {
                    string temppath = Path.Combine(destDirName, subdir.Name);
                    DirectoryCopy(subdir.FullName, temppath, copySubDirs);
                }
            }
        }

        private static void CopyFile(string fileName, string srcDir, string destDir)
        {
            File.Copy(Path.Combine(srcDir, fileName), Path.Combine(destDir, fileName), true);
        }

        private static void CopyDir(string dirName, string srcDir, string destDir)
        {
            DirectoryCopy(Path.Combine(srcDir, dirName), Path.Combine(destDir, dirName), true);
        }
    }
}
