// Copyright (c) Microsoft Corporation
// All rights reserved. 
//
// Licensed under the Apache License, Version 2.0 (the ""License""); you may not use this file except in compliance with the License. You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0 
//
// THIS CODE IS PROVIDED ON AN  *AS IS* BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION ANY IMPLIED WARRANTIES OR CONDITIONS OF TITLE, FITNESS FOR A PARTICULAR PURPOSE, MERCHANTABLITY OR NON-INFRINGEMENT. 
//
// See the Apache Version 2.0 License for specific language governing permissions and limitations under the License.

namespace NodeRTUI
{
    partial class MainForm
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
            this.label1 = new System.Windows.Forms.Label();
            this.txtFilename = new System.Windows.Forms.TextBox();
            this.cmdOpenFile = new System.Windows.Forms.Button();
            this.openFileDialog = new System.Windows.Forms.OpenFileDialog();
            this.btnGenerate = new System.Windows.Forms.Button();
            this.namespaceList = new System.Windows.Forms.ListBox();
            this.txtFilter = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.winMdBaseFolderBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.projectConfigurationRootBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.outputDirBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.cmbWindowsVersion = new System.Windows.Forms.ComboBox();
            this.lblWindowsVer = new System.Windows.Forms.Label();
            this.btnProjectGenerationDirBrowse = new System.Windows.Forms.Button();
            this.projectGenerationDirBrowserDialog = new System.Windows.Forms.FolderBrowserDialog();
            this.label6 = new System.Windows.Forms.Label();
            this.btnOutputDirBrowse = new System.Windows.Forms.Button();
            this.txtOutputDirectory = new System.Windows.Forms.TextBox();
            this.chkDefGen = new System.Windows.Forms.CheckBox();
            this.chkBuildModule = new System.Windows.Forms.CheckBox();
            this.lblVsVersion = new System.Windows.Forms.Label();
            this.cmbVsVersion = new System.Windows.Forms.ComboBox();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(42, 23);
            this.label1.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(120, 25);
            this.label1.TabIndex = 0;
            this.label1.Text = "WinMd File";
            // 
            // txtFilename
            // 
            this.txtFilename.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFilename.Location = new System.Drawing.Point(182, 17);
            this.txtFilename.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.txtFilename.Name = "txtFilename";
            this.txtFilename.ReadOnly = true;
            this.txtFilename.Size = new System.Drawing.Size(884, 31);
            this.txtFilename.TabIndex = 1;
            // 
            // cmdOpenFile
            // 
            this.cmdOpenFile.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cmdOpenFile.Location = new System.Drawing.Point(1018, 17);
            this.cmdOpenFile.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.cmdOpenFile.Name = "cmdOpenFile";
            this.cmdOpenFile.Size = new System.Drawing.Size(52, 40);
            this.cmdOpenFile.TabIndex = 3;
            this.cmdOpenFile.Text = "...";
            this.cmdOpenFile.UseVisualStyleBackColor = true;
            this.cmdOpenFile.Click += new System.EventHandler(this.cmdOpenFile_Click);
            // 
            // openFileDialog
            // 
            this.openFileDialog.DefaultExt = "winmd";
            this.openFileDialog.Filter = "WinMd files|*.winmd|WinRT dll files|*.dll|All files|*.*";
            this.openFileDialog.Title = "Select WinMd file";
            // 
            // btnGenerate
            // 
            this.btnGenerate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.btnGenerate.Location = new System.Drawing.Point(48, 848);
            this.btnGenerate.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.btnGenerate.Name = "btnGenerate";
            this.btnGenerate.Size = new System.Drawing.Size(316, 44);
            this.btnGenerate.TabIndex = 4;
            this.btnGenerate.Text = "Generate and build module";
            this.btnGenerate.UseVisualStyleBackColor = true;
            this.btnGenerate.Click += new System.EventHandler(this.cmdGenerate_Click);
            // 
            // namespaceList
            // 
            this.namespaceList.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.namespaceList.FormattingEnabled = true;
            this.namespaceList.ItemHeight = 25;
            this.namespaceList.Location = new System.Drawing.Point(48, 117);
            this.namespaceList.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.namespaceList.Name = "namespaceList";
            this.namespaceList.Size = new System.Drawing.Size(1018, 454);
            this.namespaceList.TabIndex = 5;
            // 
            // txtFilter
            // 
            this.txtFilter.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFilter.Location = new System.Drawing.Point(112, 73);
            this.txtFilter.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.txtFilter.Name = "txtFilter";
            this.txtFilter.Size = new System.Drawing.Size(954, 31);
            this.txtFilter.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(42, 79);
            this.label2.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(60, 25);
            this.label2.TabIndex = 7;
            this.label2.Text = "Filter";
            // 
            // cmbWindowsVersion
            // 
            this.cmbWindowsVersion.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbWindowsVersion.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbWindowsVersion.Items.AddRange(new object[] {
            "Windows 8",
            "Windows 8.1",
            "Windows 10"});
            this.cmbWindowsVersion.Location = new System.Drawing.Point(384, 646);
            this.cmbWindowsVersion.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.cmbWindowsVersion.Name = "cmbWindowsVersion";
            this.cmbWindowsVersion.Size = new System.Drawing.Size(682, 33);
            this.cmbWindowsVersion.TabIndex = 27;
            this.cmbWindowsVersion.SelectedIndexChanged += new System.EventHandler(this.cmbWindowsVersion_SelectedIndexChanged);
            // 
            // lblWindowsVer
            // 
            this.lblWindowsVer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblWindowsVer.AutoSize = true;
            this.lblWindowsVer.Location = new System.Drawing.Point(42, 656);
            this.lblWindowsVer.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.lblWindowsVer.Name = "lblWindowsVer";
            this.lblWindowsVer.Size = new System.Drawing.Size(233, 25);
            this.lblWindowsVer.TabIndex = 28;
            this.lblWindowsVer.Text = "Windows SDK Version:";
            // 
            // btnProjectGenerationDirBrowse
            // 
            this.btnProjectGenerationDirBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnProjectGenerationDirBrowse.Location = new System.Drawing.Point(517, 550);
            this.btnProjectGenerationDirBrowse.Name = "btnProjectGenerationDirBrowse";
            this.btnProjectGenerationDirBrowse.Size = new System.Drawing.Size(26, 21);
            this.btnProjectGenerationDirBrowse.TabIndex = 22;
            this.btnProjectGenerationDirBrowse.Text = "...";
            this.btnProjectGenerationDirBrowse.UseVisualStyleBackColor = true;
            this.btnProjectGenerationDirBrowse.Click += new System.EventHandler(this.btnOutputDirBrowse_Click);
            // 
            // label6
            // 
            this.label6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(42, 800);
            this.label6.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(171, 25);
            this.label6.TabIndex = 29;
            this.label6.Text = "Output directory:";
            // 
            // btnOutputDirBrowse
            // 
            this.btnOutputDirBrowse.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOutputDirBrowse.Location = new System.Drawing.Point(1018, 787);
            this.btnOutputDirBrowse.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.btnOutputDirBrowse.Name = "btnOutputDirBrowse";
            this.btnOutputDirBrowse.Size = new System.Drawing.Size(52, 40);
            this.btnOutputDirBrowse.TabIndex = 31;
            this.btnOutputDirBrowse.Text = "...";
            this.btnOutputDirBrowse.UseVisualStyleBackColor = true;
            this.btnOutputDirBrowse.Click += new System.EventHandler(this.btnOutputDirBrowse_Click);
            // 
            // txtOutputDirectory
            // 
            this.txtOutputDirectory.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtOutputDirectory.Location = new System.Drawing.Point(382, 787);
            this.txtOutputDirectory.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.txtOutputDirectory.Name = "txtOutputDirectory";
            this.txtOutputDirectory.ReadOnly = true;
            this.txtOutputDirectory.Size = new System.Drawing.Size(684, 31);
            this.txtOutputDirectory.TabIndex = 30;
            // 
            // chkDefGen
            // 
            this.chkDefGen.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.chkDefGen.AutoSize = true;
            this.chkDefGen.Checked = true;
            this.chkDefGen.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkDefGen.Location = new System.Drawing.Point(48, 702);
            this.chkDefGen.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.chkDefGen.Name = "chkDefGen";
            this.chkDefGen.Size = new System.Drawing.Size(529, 29);
            this.chkDefGen.TabIndex = 32;
            this.chkDefGen.Text = "Generate TypeScript and JavaScript definition files";
            this.chkDefGen.UseVisualStyleBackColor = true;
            this.chkDefGen.CheckedChanged += new System.EventHandler(this.chkDefGen_CheckedChanged);
            // 
            // chkBuildModule
            // 
            this.chkBuildModule.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.chkBuildModule.AutoSize = true;
            this.chkBuildModule.Checked = true;
            this.chkBuildModule.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkBuildModule.Location = new System.Drawing.Point(48, 750);
            this.chkBuildModule.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.chkBuildModule.Name = "chkBuildModule";
            this.chkBuildModule.Size = new System.Drawing.Size(168, 29);
            this.chkBuildModule.TabIndex = 33;
            this.chkBuildModule.Text = "Build module";
            this.chkBuildModule.UseVisualStyleBackColor = true;
            this.chkBuildModule.CheckedChanged += new System.EventHandler(this.chkBuildModule_CheckedChanged);
            // 
            // lblVsVersion
            // 
            this.lblVsVersion.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.lblVsVersion.AutoSize = true;
            this.lblVsVersion.Location = new System.Drawing.Point(44, 598);
            this.lblVsVersion.Margin = new System.Windows.Forms.Padding(6, 0, 6, 0);
            this.lblVsVersion.Name = "lblVsVersion";
            this.lblVsVersion.Size = new System.Drawing.Size(223, 25);
            this.lblVsVersion.TabIndex = 35;
            this.lblVsVersion.Text = "Visual Studio Version:";
            // 
            // cmbVsVersion
            // 
            this.cmbVsVersion.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cmbVsVersion.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cmbVsVersion.Items.AddRange(new object[] {
            "VS 2012 Project",
            "VS 2013 Project",
            "VS 2015 Project",
            "VS 2017 Project"});
            this.cmbVsVersion.Location = new System.Drawing.Point(386, 588);
            this.cmbVsVersion.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.cmbVsVersion.Name = "cmbVsVersion";
            this.cmbVsVersion.Size = new System.Drawing.Size(682, 33);
            this.cmbVsVersion.TabIndex = 34;
            this.cmbVsVersion.SelectedIndexChanged += new System.EventHandler(this.cmbVsVersion_SelectedIndexChanged);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(12F, 25F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1116, 915);
            this.Controls.Add(this.lblVsVersion);
            this.Controls.Add(this.cmbVsVersion);
            this.Controls.Add(this.chkBuildModule);
            this.Controls.Add(this.chkDefGen);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.btnOutputDirBrowse);
            this.Controls.Add(this.txtOutputDirectory);
            this.Controls.Add(this.lblWindowsVer);
            this.Controls.Add(this.cmbWindowsVersion);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.txtFilter);
            this.Controls.Add(this.namespaceList);
            this.Controls.Add(this.btnGenerate);
            this.Controls.Add(this.cmdOpenFile);
            this.Controls.Add(this.txtFilename);
            this.Controls.Add(this.label1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(6, 6, 6, 6);
            this.MinimumSize = new System.Drawing.Size(1104, 781);
            this.Name = "MainForm";
            this.Text = "NodeRT Modules Generator";
            this.FormClosed += new System.Windows.Forms.FormClosedEventHandler(this.MainForm_FormClosed);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox txtFilename;
        private System.Windows.Forms.Button cmdOpenFile;
        private System.Windows.Forms.OpenFileDialog openFileDialog;
        private System.Windows.Forms.Button btnGenerate;
        private System.Windows.Forms.ListBox namespaceList;
        private System.Windows.Forms.TextBox txtFilter;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.FolderBrowserDialog winMdBaseFolderBrowserDialog;
        private System.Windows.Forms.FolderBrowserDialog projectConfigurationRootBrowserDialog;
        private System.Windows.Forms.FolderBrowserDialog outputDirBrowserDialog;
        private System.Windows.Forms.ComboBox cmbWindowsVersion;
        private System.Windows.Forms.Label lblWindowsVer;
        private System.Windows.Forms.Button btnProjectGenerationDirBrowse;
        private System.Windows.Forms.FolderBrowserDialog projectGenerationDirBrowserDialog;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Button btnOutputDirBrowse;
        private System.Windows.Forms.TextBox txtOutputDirectory;
        private System.Windows.Forms.CheckBox chkDefGen;
        private System.Windows.Forms.CheckBox chkBuildModule;
        private System.Windows.Forms.Label lblVsVersion;
        private System.Windows.Forms.ComboBox cmbVsVersion;
    }
}

