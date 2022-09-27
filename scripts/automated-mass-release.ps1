# Function Get-Folder($initialDirectory) {
# 	[System.Reflection.Assembly]::LoadWithPartialName("System.windows.forms") | Out-Null

# 	$foldername = New-Object System.Windows.Forms.FolderBrowserDialog
# 	$foldername.Description = "Select a folder to relase from"
# 	$foldername.rootfolder = "MyComputer"

# 	if ($foldername.ShowDialog() -eq "OK") {
# 		$folder += $foldername.SelectedPath
# 	}
# 	return $folder
# }

$dir = $ExecutionContext.SessionState.Path.GetUnresolvedProviderPathFromPSPath("$PSScriptRoot\..\NodeRT-Bindings")

# if ($dir -eq $null) {
# 	return
# }
if (-Not (Test-Path -Path $dir)) {
	return
}

$dirs = Get-ChildItem $dir

foreach ($d in $dirs) {
	Write-Host "Publishing $d"

	cd $d.FullName
  
	$dryRun = "false"
	if ((Test-Path env:\npmDryRun) -And ($Env:npmDryRun -eq "true")) {
		$dryRun = "true"
	}

	npm publish . --access public --dry-run $dryRun
	cd $dir
}
