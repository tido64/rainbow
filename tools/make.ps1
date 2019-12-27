# Copyright (c) 2010-present Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

param (
	[switch]$Clean,
	[switch]$Generate,
	[switch]$Help
)

$SourcePath = Join-Path (Split-Path (Get-Variable MyInvocation).Value.MyCommand.Path) .. -Resolve

$FMODDescription = "Enable FMOD Studio audio engine"
$HeimdallDescription = "Enable Heimdall debugging facilities"
$PhysicsDescription = "Enable physics module (Box2D)"
$UnitTestsDescription = "Enable unit tests"

function Make
{
	param ($options)

	cmake $options -A x64 $SourcePath
	.\Rainbow.sln
}

if ($Help) {
	$Script = $MyInvocation.InvocationName
	Write-Output "Syntax: $Script [-Clean|-Generate [option ...]]"
	Write-Output ""
	Write-Output "Options:"
	Write-Output "  -DUNIT_TESTS=1           $UnitTestsDescription"
	Write-Output "  -DUSE_FMOD_STUDIO=1      $FMODDescription"
	Write-Output "  -DUSE_HEIMDALL=1         $HeimdallDescription"
	Write-Output "  -DUSE_PHYSICS=1          $PhysicsDescription"
	Write-Output ""
	Write-Output "CMake options are passed directly to CMake so you can set variables like"
	Write-Output "-DCMAKE_BUILD_TYPE=<type> among others."
}
elseif ($Clean) {
	Remove-Item * -include *.dir,*.vcxproj,*.vcxproj.filters,*.vcxproj.user,cmake_install.cmake,CMakeCache.txt,CMakeFiles,Debug,ipch,lib,MinSizeRel,Rainbow.sdf,Rainbow.sln,Release,RelWithDebInfo,Win32 -recurse
}
elseif ($Generate) {
	Make "$args"
}
else {
	Add-Type -AssemblyName System.Windows.Forms

	$UnitTestsCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $UnitTestsDescription
	}
	$FMODCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $FMODDescription
	}
	$HeimdallCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $HeimdallDescription
	}
	$PhysicsCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $PhysicsDescription
	}

	$AcceptButton = New-Object System.Windows.Forms.Button -Property @{
		DialogResult = [System.Windows.Forms.DialogResult]::OK
		FlatStyle = [System.Windows.Forms.FlatStyle]::System
		Text = "Generate"
	}
	$CancelButton = New-Object System.Windows.Forms.Button -Property @{
		DialogResult = [System.Windows.Forms.DialogResult]::Cancel
		FlatStyle = [System.Windows.Forms.FlatStyle]::System
		Text = "Cancel"
	}
	$ButtonLayout = New-Object System.Windows.Forms.FlowLayoutPanel -Property @{
		AutoSize = $true
		Anchor = [System.Windows.Forms.AnchorStyles]::Right
	}
	$ButtonLayout.Controls.AddRange(($AcceptButton, $CancelButton));

	$Layout = New-Object System.Windows.Forms.FlowLayoutPanel -Property @{
		AutoSize = $true
		FlowDirection = [System.Windows.Forms.FlowDirection]::TopDown
		Padding = 4
		WrapContents = $false
	}
	$Layout.Controls.AddRange((
		$UnitTestsCheckBox,
		$FMODCheckBox,
		$HeimdallCheckBox,
		$PhysicsCheckBox,
		$ButtonLayout
	));

	$Form = New-Object System.Windows.Forms.Form -Property @{
		AcceptButton = $AcceptButton
		AutoScroll = $false
		AutoSize = $true
		AutoSizeMode = [System.Windows.Forms.AutoSizeMode]::GrowAndShrink
		CancelButton = $CancelButton
		FormBorderStyle = [System.Windows.Forms.FormBorderStyle]::FixedSingle
		MaximizeBox = $false
		MinimizeBox = $false
		ShowIcon = $false
		ShowInTaskbar = $false
		StartPosition = [System.Windows.Forms.FormStartPosition]::CenterScreen
		Text = "Customize your Rainbow build"
	}
	$Form.Controls.Add($Layout)

	if ($Form.ShowDialog() -ne [System.Windows.Forms.DialogResult]::OK) {
		exit
	}

	$Options = @()
	if ($UnitTestsCheckBox.Checked) {
		$Options += "-DUNIT_TESTS=1"
	}
	if ($FMODCheckBox.Checked) {
		$Options += "-DUSE_FMOD_STUDIO=1"
	}
	if ($HeimdallCheckBox.Checked) {
		$Options += "-DUSE_HEIMDALL=1"
	}
	if ($PhysicsCheckBox.Checked) {
		$Options += "-DUSE_PHYSICS=1"
	}

	Push-Location $SourcePath\build\windows
	Make $Options
	Pop-Location
}
