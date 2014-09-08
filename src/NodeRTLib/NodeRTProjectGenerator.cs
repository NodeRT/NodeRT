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
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;

namespace NodeRTLib
{
    public enum VsVersions
    {
        Vs2012,
        Vs2013
    }

    public class NodeRTProjectGenerator
    {
        private string _sourceDir;
        private VsVersions _vsVersion;
        private bool _isGenerateDef;

        public NodeRTProjectGenerator(string sourceDir, VsVersions vsVersion, bool isGenerateDef)
        {
            _sourceDir = sourceDir.TrimEnd('\\');
            _vsVersion = vsVersion;
            _isGenerateDef = isGenerateDef;
        }

        public static string DefaultDir
        {
            get
            {
                string baseDir = @"C:\Users\" + Environment.UserName + @"\.node-gyp";

                if (!Directory.Exists(baseDir))
                {
                    return String.Empty;
                }

                string [] dirs = Directory.GetDirectories(baseDir);
                if (dirs.Length == 0)
                {
                    return String.Empty;
                }

                // choose the latest version
                return Path.Combine(baseDir,dirs[dirs.Length-1]);
            }
        }

        public string GenerateProject(string winRTNamespace, string destinationFolder, string winRtFile, dynamic mainModel)
        {
            string projectName = "NodeRT_" + winRTNamespace.Replace(".", "_");

            if (!Directory.Exists(destinationFolder))
            {
                Directory.CreateDirectory(destinationFolder);
            }

            string outputFileName = "NodeRT." + winRTNamespace + ".cpp";
            using (var writer = new StreamWriter(Path.Combine(destinationFolder, outputFileName)))
            {
                writer.Write(TX.CppTemplates.Wrapper(mainModel));
            }

            if (_isGenerateDef)
            {
                string libDirPath = Path.Combine(destinationFolder, "lib");

                if (!Directory.Exists(libDirPath))
                {
                    Directory.CreateDirectory(libDirPath);
                }

                using (var writer = new StreamWriter(Path.Combine(libDirPath, projectName + ".d.js")))
                {
                    writer.Write(TX.JsDefinitionTemplates.Wrapper(mainModel));
                }

                using (var writer = new StreamWriter(Path.Combine(libDirPath, projectName + ".d.ts")))
                {
                    writer.Write(TX.TsDefinitionTemplates.Wrapper(mainModel));
                }
            }


            StringBuilder slnFileText = new StringBuilder(File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"ProjectTemplates\NodeRTSolutionTemplate.sln")));
            StringBuilder projectFileText;

            projectFileText = new StringBuilder(File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"ProjectTemplates\NodeRTProjectTemplate.vcxproj")));

            slnFileText.Replace("{ProjectName}", projectName);
            var slnPath = Path.Combine(destinationFolder, projectName + ".sln");
            File.WriteAllText(slnPath, slnFileText.ToString());

            projectFileText.Replace("{ProjectName}", projectName);
            projectFileText.Replace("{CppFileName}", outputFileName);
            projectFileText.Replace("{NodeSrcDir}", _sourceDir);

            ResolveWinrtDirsAndCompiler(projectFileText, winRtFile);

            File.WriteAllText(Path.Combine(destinationFolder, projectName + ".vcxproj"), projectFileText.ToString());

            GenerateFiltersFile(outputFileName, projectName, destinationFolder);

            CopyProjectFiles(destinationFolder);

            CopyAndGenerateJsPackageFiles(destinationFolder, winRTNamespace, projectName, mainModel);

            return slnPath;
        }

        private void GenerateFiltersFile(string cppOutputFileName, string projectName, string destinationFolder)
        {
            StringBuilder projectFiltersFileText = new StringBuilder(File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"ProjectTemplates\ProjectTemplateFilters.vcxproj.filters")));
            projectFiltersFileText.Replace("{CppFileName}", cppOutputFileName);
            File.WriteAllText(Path.Combine(destinationFolder, projectName + ".vcxproj.filters"), projectFiltersFileText.ToString());
        }

        protected void ResolveWinrtDirsAndCompiler(StringBuilder projectFileText, string winrtFile)
        {
            StringBuilder x64UsingDirs = new StringBuilder();

            string directoryName = Path.GetDirectoryName(winrtFile).ToLower();

            string programFilesDir = NodeRTProjectBuildUtils.GetProgramFilesX86Dir();
            if (_vsVersion == VsVersions.Vs2012)
            {
                projectFileText.Replace("{PlatformToolset}", "v110");
                x64UsingDirs.Append(programFilesDir + @"\Microsoft SDKs\Windows\v8.0\ExtensionSDKs\Microsoft.VCLibs\11.0\References\CommonConfiguration\neutral;" + 
                    programFilesDir + @"\Windows Kits\8.0\References\CommonConfiguration\Neutral;");
            }
            else if (_vsVersion == VsVersions.Vs2013)
            {
                projectFileText.Replace("{PlatformToolset}", "v120");
                x64UsingDirs.Append(programFilesDir + @"\Microsoft SDKs\Windows\v8.1\ExtensionSDKs\Microsoft.VCLibs\12.0\References\CommonConfiguration\neutral;" + 
                    programFilesDir + @"\Windows Kits\8.1\References\CommonConfiguration\Neutral;");
            }

            // resolve the x64 dirs using the sdk we use:
            if (!directoryName.EndsWith(@"windows kits\8.1\references\commonconfiguration\neutral") && !
                directoryName.EndsWith(@"windows kits\8.0\references\commonconfiguration\neutral"))
            {
                // add the directory to the x64 dirs:
                x64UsingDirs.Append(Path.GetDirectoryName(winrtFile));
            }

            projectFileText.Replace("{AdditionalWinRtDirsx64}", x64UsingDirs.ToString());
        }

        private void CopyAndGenerateJsPackageFiles(string destinationFolder, string winRTNamespace, string projectName, dynamic mainModel)
        {
            string libDirPath = Path.Combine(destinationFolder, "lib");
            if (!Directory.Exists(libDirPath))
            {
                Directory.CreateDirectory(libDirPath);
            }

            // write the main.js file:
            StringBuilder mainJsFileText = new StringBuilder(File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"JsPackageFiles\main.js")));
            mainJsFileText.Replace("{ProjectName}", projectName);
            mainJsFileText.Replace("{BinDir}", "process.arch");
            File.WriteAllText(Path.Combine(libDirPath, "main.js"), mainJsFileText.ToString());

            // write the README.md file
            StringBuilder readmeFileText = new StringBuilder(File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"JsPackageFiles\README.md")));
            readmeFileText.Replace("{Namespace}", winRTNamespace);
            File.WriteAllText(Path.Combine(destinationFolder, "README.md"), readmeFileText.ToString());

            // write the package.json file:
            StringBuilder packageJsonFileText = new StringBuilder(File.ReadAllText(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"JsPackageFiles\package.json")));
            packageJsonFileText.Replace("{Namespace}", winRTNamespace);
            packageJsonFileText.Replace("{PackageName}", winRTNamespace.ToLower());
            packageJsonFileText.Replace("{Keywords}", GeneratePackageKeywords(mainModel, winRTNamespace));
            File.WriteAllText(Path.Combine(destinationFolder, "package.json"), packageJsonFileText.ToString());

            // copy the .npmignore
            File.Copy(Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, @"JsPackageFiles\.npmignore"),
                Path.Combine(destinationFolder, ".npmignore"), true);
        }

        private string GeneratePackageKeywords(dynamic mainModel, string winRtNamespace)
        {
            StringBuilder keywordsBuilder = new StringBuilder();
            keywordsBuilder.Append("\"" + winRtNamespace + "\"");

            foreach (string str in winRtNamespace.Split('.'))
            {
                keywordsBuilder.Append(",\r\n    \"" + str + "\"");
            }

            foreach (var c in mainModel.Types)
            {
                keywordsBuilder.Append(",\r\n    \"" + c.Key.Name + "\"");
            }

            foreach (var e in mainModel.Enums)
            {
                keywordsBuilder.Append(",\r\n    \"" + e.Name + "\"");
            }

            foreach (var v in mainModel.ValueTypes)
            {
                keywordsBuilder.Append(",\r\n    \"" + v.Name + "\"");
            }

            return keywordsBuilder.ToString();
        }

        private void CopyProjectFiles(string destinationFolder)
        {
            string dirPath = Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, "ProjectFiles");
            string[] files = Directory.GetFiles(dirPath);

            foreach (string file in files)
            {
                try
                {
                    File.Copy(file, Path.Combine(destinationFolder, Path.GetFileName(file)), true);
                }
                catch (Exception e)
                {
                    throw e;
                }
            }
        }
    }
}
