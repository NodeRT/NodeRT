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
                chkDefGen.Checked = Properties.Settings.Default.GenerateDefsChk;
                txtFilename.Text = Properties.Settings.Default.LastWinMDPath;
                txtFilter.Text = Properties.Settings.Default.LastFilter;
                cmbVsVersion.SelectedIndex = Properties.Settings.Default.VsVersionComboSelection;
                cmbWindowsVersion.SelectedIndex = Properties.Settings.Default.WinVersionComboSelection;
                chkBuildModule.Checked = Properties.Settings.Default.BuildModuleChk;

                if (String.IsNullOrEmpty(Properties.Settings.Default.OutputDirPath))
                {
                    Properties.Settings.Default.OutputDirPath = GetDefaultOutputDir();
                    Properties.Settings.Default.Save();
                }

                txtOutputDirectory.Text = Properties.Settings.Default.OutputDirPath;

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
            Properties.Settings.Default.VsVersionComboSelection = 4;
            Properties.Settings.Default.OutputDirPath = GetDefaultOutputDir();
            Properties.Settings.Default.GenerateDefsChk = true;
            Properties.Settings.Default.Save();

            winMdBaseFolderBrowserDialog.SelectedPath = null;
            txtFilename.Text = "";
            txtFilter.Text = "";
            txtOutputDirectory.Text = Properties.Settings.Default.OutputDirPath;
            cmbWindowsVersion.SelectedIndex = 2;
            cmbVsVersion.SelectedIndex = 4;
            chkDefGen.Checked = true;
            chkBuildModule.Checked = true;
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
                        "Error details: " + ex.Message, "WinMD load error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    ClearSettingsAndUI();

                    System.Diagnostics.Process.Start(Application.ExecutablePath);
                    this.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Error occurred when loading WinMD file: \n" +
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
            VsVersions vsVersion = (VsVersions)cmbVsVersion.SelectedIndex;
            WinVersions winVersion = (WinVersions)cmbWindowsVersion.SelectedIndex;
            string outputFolder = Path.Combine(txtOutputDirectory.Text, winRTNamespace.ToLower());

            string errorMessage;
            if (!NodeRTProjectGenerator.VerifyVsAndWinVersions(winVersion, vsVersion, out errorMessage))
            {
                MessageBox.Show("Unsupported Windows and VS combination:\n" + errorMessage, "Unsupported options", MessageBoxButtons.OK, MessageBoxIcon.Error);
                btnGenerate.Enabled = true;
                return;
            }

            var generator = new NodeRTProjectGenerator(winVersion, vsVersion, chkDefGen.Checked);
            bool buildModule = chkBuildModule.Checked;

            btnGenerate.Text = "Generating code...";
            bool succeeded = false;

            Task.Run(() =>
            {
                string modulePath;
                try
                {
                    modulePath = Reflector.GenerateProject(winMdFile, winRTNamespace, outputFolder,
                        generator, null, null, null);
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Failed to generate project file\n" + ex.Message,
                        "Failed to generate project file", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    throw ex;
                }

                MethodInvoker invoker = new MethodInvoker(delegate() { btnGenerate.Text = "Building..."; });
                btnGenerate.Invoke(invoker);

                if (buildModule)
                {
                    try
                    {
                        NodeRTProjectBuildUtils.BuildWithNodeGyp(modulePath, vsVersion);
                        succeeded = true;
                    }
                    catch (IOException ex)
                    {
                        MessageBox.Show("IO Error occured after building the project:\n" +
                            ex.Message + "\n" +
                            "You can access the project files at: " + outputFolder, "IO Error occurred", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                    catch (Exception)
                    {
                        MessageBox.Show("Failed to build the project from generated code.\n" +
                            "Please try to build the project manually.\n" +
                            "You can access the project files at: " + outputFolder, "Failed to build project", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    }
                }
                else
                {
                    succeeded = true;
                }
            })
            .ContinueWith((t) =>
            {
                btnGenerate.Enabled = true;
                if (chkBuildModule.Checked)
                    btnGenerate.Text = "Generate and build module";
                else
                    btnGenerate.Text = "Generate module";

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

        private void btnOutputDirBrowse_Click(object sender, EventArgs e)
        {
            outputDirBrowserDialog.SelectedPath = txtOutputDirectory.Text;
            var result = outputDirBrowserDialog.ShowDialog();
            if (result != System.Windows.Forms.DialogResult.Cancel)
            {
                Properties.Settings.Default.OutputDirPath = outputDirBrowserDialog.SelectedPath;
                Properties.Settings.Default.Save();
                txtOutputDirectory.Text = outputDirBrowserDialog.SelectedPath;
            }
        }

        private void chkDefGen_CheckedChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.GenerateDefsChk = chkDefGen.Checked;
            Properties.Settings.Default.Save();
        }

        private void chkBuildModule_CheckedChanged(object sender, EventArgs e)
        {
            if (chkBuildModule.Checked)
            {
                btnGenerate.Text = "Generate and build module";
            }
            else
            {
                btnGenerate.Text = "Generate module";
            }
        }

        private void cmbVsVersion_SelectedIndexChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.VsVersionComboSelection = cmbVsVersion.SelectedIndex;
            Properties.Settings.Default.Save();
        }

        private void cmbWindowsVersion_SelectedIndexChanged(object sender, EventArgs e)
        {
            Properties.Settings.Default.WinVersionComboSelection = cmbWindowsVersion.SelectedIndex;
            Properties.Settings.Default.Save();
        }
    }
}
