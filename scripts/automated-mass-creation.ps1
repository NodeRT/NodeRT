# Param(
#   [Parameter(Mandatory=$true)]
#   [string]$npmVersion
# )

# Function Get-Folder {
# 	$description = $args[0]
# 	$initialPath = $args[1]

# 	[System.Reflection.Assembly]::LoadWithPartialName("System.windows.forms") | Out-Null;
# 	[System.Windows.Forms.Application]::EnableVisualStyles();

# 	$foldername = New-Object System.Windows.Forms.FolderBrowserDialog;
# 	$foldername.Description = $description;
# 	#$foldername.rootfolder = "MyComputer";

# 	if (Test-Path $initialPath) {
# 		$foldername.SelectedPath = $initialPath;
# 	}

# 	if ($foldername.ShowDialog() -eq "OK") {
# 		$folder += $foldername.SelectedPath;
# 	}

# 	return $folder;
# }

$nodertCmd = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath("$PSScriptRoot\..\src\NodeRTCmd\bin\Debug\NodeRTCmd.exe");

if ((Test-Path -Path $nodertCmd) -eq $false) {
	Write-Host "Could not find $nodertCmd.";
	Write-Host "Now exiting.";
	return;
}

# $unionMetadataDir = Get-Folder "Select your Windows SDK UnionMetadata folder" "C:\Program Files (x86)\Windows Kits\10\UnionMetadata";
$unionMetadataDir = "C:\Program Files (x86)\Windows Kits\10\UnionMetadata";

# if ($unionMetadataDir -eq $null) {
#   return;
# }

$unionMetadataDirs = Get-ChildItem -dir $unionMetadataDir -Filter "10.*"
$sdks = @{};

foreach ($d in $unionMetadataDirs) {
	$namespace = ""

	Switch ($d.Name) {
		"10.0.15063.0" { $namespace = "nodert-win10-cu"; }
		"10.0.16299.0" { $namespace = "nodert-win10-rs3"; }
		"10.0.17134.0" { $namespace = "nodert-win10-rs4"; }
		"10.0.17763.0" { $namespace = "nodert-win10-rs5"; }
		"10.0.18362.0" { $namespace = "nodert-win10-19h1"; }
		"10.0.19041.0" { $namespace = "nodert-win10-20h1"; }
		"10.0.20348.0" { $namespace = "nodert-win10-21h1"; }

		"10.0.22000.0" { $namespace = "nodert-win11"; }
		"10.0.22621.0" { $namespace = "nodert-win11-22h2"; }
	}

	if ($namespace -eq "") {
		Write-Host "Found SDK folder $d, but it's unknown. We won't use it.";
	} else {
		Write-Host "Will generate NodeRT modules using metadata found in $d (as $namespace)"

		$sdks[$namespace] = $d.FullName;
	}
}

# $reply = Read-Host -Prompt "Continue? [y/n]"
# if ($reply -match "[yY]") {
foreach ($sdk in $sdks.keys) {
	$sdkFolder = $sdks.$sdk;
	Write-Host "Generating packages for @$sdk using $sdkFolder";

	# $outDir = New-Item -Path "~/Desktop/NodeRT/$sdk" -ItemType directory;
	$outDir = New-Item -Path "../NodeRT-Bindings/$sdk" -ItemType directory;
	Write-Host "Output will be available in $outDir"

	& $nodertCmd --winmd $sdkFolder\Windows.winmd --outdir $outDir --npmscope $sdk --npmversion $Env:npmVersion --nobuild
}
# }