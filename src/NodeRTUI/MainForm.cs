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
using System.ComponentModel;
using System.Data;
using System.Drawing;

using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace NodeRTUI
{
    using System.Diagnostics;
    using System.Reactive;
    using System.Reactive.Linq;
    using System.Threading;
    using System.Configuration;

    using NodeRTLib;
    using System.IO;

    public partial class MainForm : Form
    {
        private IEnumerable<string> _namespaces;

        public MainForm()
        {
            InitializeComponent();

            var queryTextChangedObservable =
             Observable.FromEventPattern<EventHandler, EventArgs>
               (s => txtFilter.TextChanged += s, s => txtFilter.TextChanged -= s);

            queryTextChangedObservable
                .Throttle(TimeSpan.FromMilliseconds(100))
                .Scan(new { cts = new CancellationTokenSource(), e = default(EventPattern<EventArgs>) },
                    (previous, newObj) =>
                    {
                        previous.cts.Cancel();
                        return new { cts = new CancellationTokenSource(), e = newObj };
                    })
                .ObserveOn(SynchronizationContext.Current)
                .Subscribe(s =>
                    {
                        FilterNamespaces();
                    });

            try
            {
                txtFilename.Text = Properties.Settings.Default.LastWinMDPath;
                txtFilter.Text = Properties.Settings.Default.LastFilter;
                cmbTargetVs.SelectedIndex = Properties.Settings.Default.VSProjectComboSelection;
                txtProjectGenerationDirectory.Text = Properties.Settings.Default.LastSavedFolder;
                if (String.IsNullOrEmpty(Properties.Settings.Default.ProjectGenerationDir))
                {
                    Properties.Settings.Default.ProjectGenerationDir = GetDefaultCodeGenerationDir();
                    Properties.Settings.Default.Save();
                }

                txtProjectGenerationDirectory.Text = Properties.Settings.Default.ProjectGenerationDir;

                if (String.IsNullOrEmpty(Properties.Settings.Default.OutputDirPath))
                {
                    Properties.Settings.Default.OutputDirPath = GetDefaultOutputDir();
                    Properties.Settings.Default.Save();
                }

                txtOutputDirectory.Text = Properties.Settings.Default.OutputDirPath;

                if (String.IsNullOrEmpty(Properties.Settings.Default.ProjectConfigurationRoot))
                {
                    Properties.Settings.Default.ProjectConfigurationRoot = NodeRTProjectGenerator.DefaultDir;
                    Properties.Settings.Default.Save();
                }

                txtProjectConfigurationRoot.Text = Properties.Settings.Default.ProjectConfigurationRoot;

                if (!string.IsNullOrWhiteSpace(txtFilename.Text))
                {
                    LoadNamespaces(txtFilename.Text);
                }
            }
            catch
            {
                // failed...reset everything..
                ClearSettingsAndUI();
            }
        }

        private void ClearSettingsAndUI()
        {
            Properties.Settings.Default.LastWinMDPath = "";
            Properties.Settings.Default.LastFilter = "";
            Properties.Settings.Default.ProjectConfigurationRoot = NodeRTProjectGenerator.DefaultDir;
            Properties.Settings.Default.VSProjectComboSelection = 1;
            Properties.Settings.Default.LastSavedFolder = "";
            Properties.Settings.Default.ProjectGenerationDir = GetDefaultCodeGenerationDir();
            Properties.Settings.Default.OutputDirPath = GetDefaultOutputDir();
            Properties.Settings.Default.Save();

            winMdBaseFolderBrowserDialog.SelectedPath = null;
            txtFilename.Text = "";
            txtFilter.Text = "";
            txtProjectGenerationDirectory.Text = Properties.Settings.Default.ProjectGenerationDir;
            txtOutputDirectory.Text = Properties.Settings.Default.OutputDirPath;
            txtProjectConfigurationRoot.Text = Properties.Settings.Default.ProjectConfigurationRoot;
            cmbTargetVs.SelectedIndex = 0;
            namespaceList.Items.Clear();
        }

        private string GetDefaultCodeGenerationDir()
        {
            return Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, "gen_code");
        }

        private string GetDefaultOutputDir()
        {
            return Path.Combine(AppDomain.CurrentDomain.SetupInformation.ApplicationBase, "output");
        }

        private void FilterNamespaces()
        {
            if (_namespaces != null)
            {
                if (!string.IsNullOrWhiteSpace(txtFilter.Text))
                {
                    var filter = txtFilter.Text.Trim().ToLowerInvariant();
                    var filteredNamespaces = _namespaces.Where((name) => name.ToLowerInvariant().Contains(filter));
                    namespaceList.SuspendLayout();
                    namespaceList.Items.Clear();
                    namespaceList.Items.AddRange(filteredNamespaces.ToArray());
                    namespaceList.ResumeLayout();
                }
                else
                {
                    namespaceList.Items.AddRange(_namespaces.ToArray());
                }
            }
        }

        private void LoadNamespaces(string path)
        {
            _namespaces = Reflector.GetNamespaces(path, null).OrderBy((s) => s);
            FilterNamespaces();
        }

        private void cmdOpenFile_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrWhiteSpace(Properties.Settings.Default.LastWinMDPath))
            {
                openFileDialog.InitialDirectory = Path.GetDirectoryName(Properties.Settings.Default.LastWinMDPath);
            }

            var result = openFileDialog.ShowDialog();
            if (result != System.Windows.Forms.DialogResult.Cancel)
            {
                namespaceList.Items.Clear();
                try
                {
                    LoadNamespaces(openFileDialog.FileName);

                    txtFilename.Text = openFileDialog.FileName;
                    Properties.Settings.Default.LastWinMDPath = openFileDialog.FileName;
                    Properties.Settings.Default.Save();

                }
                catch (TypeLoadException ex)
                {
                    MessageBox.Show("Failed to load winmetadata information! \n" +
                        "Error details: " + ex.Message, "winmd load error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    ClearSettingsAndUI();

                    System.Diagnostics.Process.Start(Application.ExecutablePath);
                    this.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error occured when loading WinMD file: \n" +
                        ex.Message, "Failed to load WinMd file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void cmdGenerate_Click(object sender, EventArgs e)
        {
            // in case we have a single item just select it
            if (namespaceList.Items.Count == 1)
            {
                namespaceList.SelectedItem = namespaceList.Items[0];
            }

            if ((namespaceList.SelectedItem == null) || string.IsNullOrEmpty(txtFilename.Text))
            {
                MessageBox.Show("Please select a namespace to generate", "No namespace was chosen");
                return;
            }

            btnGenerate.Enabled = false;

            var winMdFile = txtFilename.Text;
            var winRTNamespace = namespaceList.SelectedItem.ToString();
            VsVersions vsVersion = (VsVersions)cmbTargetVs.SelectedIndex;
            
            NodeRTProjectBuildUtils.Platforms platforms = NodeRTProjectBuildUtils.Platforms.Win32;
            if (NodeRTProjectBuildUtils.IsRunningOn64Bit)
             platforms|= NodeRTProjectBuildUtils.Platforms.x64;

            string codeGenerationFolder = Path.Combine(txtProjectGenerationDirectory.Text, winRTNamespace.ToLower());
            string outputFolder = Path.Combine(txtOutputDirectory.Text, winRTNamespace.ToLower());

            var generator = new NodeRTProjectGenerator(txtProjectConfigurationRoot.Text, vsVersion, chkDefGen.Checked);

            btnGenerate.Text = "Generating code...";
            bool succeeded = false;

            Task.Run(() =>
            {
                string slnPath;
                try
                {
                    slnPath = Reflector.GenerateProject(winMdFile, winRTNamespace, codeGenerationFolder,
                        generator, null);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Failed to generate project file\n" + ex.Message,
                        "Failed to generate project file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    throw ex;
                }

                MethodInvoker invoker = new MethodInvoker(delegate() { btnGenerate.Text = "Building..."; });
                btnGenerate.Invoke(invoker);

                try
                {
                    NodeRTProjectBuildUtils.BuildAndCopyToOutputFolder(slnPath, vsVersion, outputFolder, platforms, chkDefGen.Checked);
                    succeeded = true;
                }
                catch (IOException ex)
                {
                    MessageBox.Show("IO Error occured after building the project:\n" +
                        ex.Message + "\n" +
                        "You can access the project files at: " + codeGenerationFolder, "IO Error occured", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
                catch (Exception)
                {
                    MessageBox.Show("Failed to build the project from genreated code.\n" +
                        "Please try to build the project manually.\n" +
                        "You can access the project files at: " + codeGenerationFolder, "Failed to build project", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            })
            .ContinueWith((t) =>
            {
                btnGenerate.Enabled = true;
                btnGenerate.Text = "Generate and Build";

                if (succeeded)
                    MessageBox.Show("Yay! The generated NodeRT module is located at:\n" + outputFolder, "Success");
            },
            TaskScheduler.FromCurrentSynchronizationContext());
        }

        private void MainForm_FormClosed(object sender, FormClosedEventArgs e)
        {
            Properties.Settings.Default.LastFilter = txtFilter.Text;
            Properties.Settings.Default.Save();
        }

        private void btnBrowseProjectConfigurationRoot_Click(object sender, EventArgs e)
        {
            projectConfigurationRootBrowserDialog.SelectedPath = Properties.Settings.Default.ProjectConfigurationRoot;

            var result = projectConfigurationRootBrowserDialog.ShowDialog();
            if (result != System.Windows.Forms.DialogResult.Cancel)
            {
                Properties.Settings.Default.ProjectConfigurationRoot = projectConfigurationRootBrowserDialog.SelectedPath;
                Properties.Settings.Default.Save();
                txtProjectConfigurationRoot.Text = projectConfigurationRootBrowserDialog.SelectedPath;
            }
        }

        private void btnNodeFilesRootDefault_Click(object sender, EventArgs e)
        {
            txtProjectConfigurationRoot.Text = NodeRTProjectGenerator.DefaultDir;

            Properties.Settings.Default.ProjectConfigurationRoot = txtProjectConfigurationRoot.Text;
            Properties.Settings.Default.Save();
        }

        private void btnProjectGenerationDirBrowse_Click(object sender, EventArgs e)
        {
            projectGenerationDirBrowserDialog.SelectedPath = txtProjectGenerationDirectory.Text;
            var result = projectGenerationDirBrowserDialog.ShowDialog();
            if (result != System.Windows.Forms.DialogResult.Cancel)
            {
                Properties.Settings.Default.ProjectGenerationDir = projectGenerationDirBrowserDialog.SelectedPath;
                Properties.Settings.Default.Save();
                txtProjectGenerationDirectory.Text = projectGenerationDirBrowserDialog.SelectedPath;
            }
        }

        private void btnOutputDirBrowse_Click(object sender, EventArgs e)
        {
            outputDirBrowserDialog.SelectedPath = txtProjectGenerationDirectory.Text;
            var result = outputDirBrowserDialog.ShowDialog();
            if (result != System.Windows.Forms.DialogResult.Cancel)
            {
                Properties.Settings.Default.OutputDirPath = outputDirBrowserDialog.SelectedPath;
                Properties.Settings.Default.Save();
                txtProjectGenerationDirectory.Text = outputDirBrowserDialog.SelectedPath;
            }
        }
    }
}
