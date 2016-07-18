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
        private const string NODE_GYP_CMD_TEMPLATE = "\"cd \"{0}\" & node-gyp rebuild --msvs_version={1}\"";

        // Builds the given project/sln for the given platforms and copies the output & package file to the output directory
        public static void BuildWithNodeGyp(string moduleDirectory, VsVersions vsVersion)
        {
            BuildModule(moduleDirectory, vsVersion);
        }

        private static string CreateBuildCmd(string moduleDirectory, VsVersions vsVersion)
        {
            string versionString;

            switch (vsVersion)
            {
                case VsVersions.Vs2012:
                    versionString = "2012";
                    break;
                case VsVersions.Vs2013:
                    versionString = "2013";
                    break;
                case VsVersions.Vs2015:
                    versionString = "2015";
                    break;
                default:
                    throw new Exception("Unknown VS Version");
            }
            return String.Format(NODE_GYP_CMD_TEMPLATE, moduleDirectory, versionString);
        }

        private static void BuildModule(string moduleDirectory, VsVersions vsVersion)
        {
            string cmd = CreateBuildCmd(moduleDirectory, vsVersion);
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
    }
}
