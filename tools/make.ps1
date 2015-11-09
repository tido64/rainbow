# Copyright (c) 2010-15 Bifrost Entertainment AS and Tommy Nguyen
# Distributed under the MIT License.
# (See accompanying file LICENSE or copy at http://opensource.org/licenses/MIT)

param (
	[switch]$Clean,
	[switch]$Generate,
	[switch]$Help)

$Generator = "Visual Studio 14"
$SourcePath = Join-Path (Split-Path (Get-Variable MyInvocation).Value.MyCommand.Path) .. -Resolve
$LibraryPath = Join-Path $SourcePath lib
$PackagesPath = "Packages"

$FMODDescription = "Enable FMOD Studio audio engine"
$HeimdallDescription = "Enable Heimdall debugging facilities"
$PhysicsDescription = "Enable physics module (Box2D)"
$SpineDescription = "Enable Spine runtime"
$UnitTestsDescription = "Enable unit tests"
$VectorDescription = "Enable vector drawing library (NanoVG)"

$Shell = New-Object -ComObject Shell.Application

function Get-Package
{
	param ($Uri, $OutFile)

	$Packages = New-Item -Path $PackagesPath -ItemType Directory -Force

	Write-Host "Downloading $Uri..."
	$OutFile = [IO.Path]::Combine($Packages.FullName, $OutFile)
	Invoke-WebRequest -Uri $Uri -UserAgent $UserAgent -OutFile $OutFile

	if (Test-Path $OutFile) {
		Write-Host "Extracting $OutFile..."
		$Packages = $Shell.NameSpace($Packages.FullName)
		foreach ($Item in $Shell.NameSpace($OutFile).Items()) {
			$Packages.CopyHere($Item)
		}
	}
}

function Get-AllPackages
{
	if (!(Test-Path (Join-Path $SourcePath "lib/SDL/include/SDL.h")) -or !(Test-Path (Join-Path $SourcePath "lib/SDL/lib/x86/SDL2.lib"))) {
		$SDL2 = "SDL2-devel-2.0.4-VC.zip"
		Get-Package "http://www.libsdl.org/tmp/release/$SDL2" $SDL2
		$SDL2 = Join-Path $PackagesPath SDL2-2.0.4 -Resolve
		if (Test-Path $SDL2) {
			Move-Item $SDL2 ([IO.Path]::Combine($LibraryPath, "SDL"))
		}
	}
	if (!(Test-Path (Join-Path $SourcePath "lib/openal-soft/include/AL/al.h")) -or !(Test-Path (Join-Path $SourcePath "lib/openal-soft/libs/Win32/OpenAL32.lib"))) {
		$AL = "openal-soft-1.17.0-bin.zip"
		Get-Package "http://kcat.strangesoft.net/openal-binaries/$AL" $AL
		$AL = Join-Path $PackagesPath openal-soft-1.17.0-bin -Resolve
		if (Test-Path $AL) {
			Move-Item $AL ([IO.Path]::Combine($LibraryPath, "openal-soft"))
		}
	}
}

function Make
{
	param ($options)

	Get-AllPackages
	cmake $options -G $Generator $SourcePath
	.\Rainbow.sln
}

if ($Clean) {
	Remove-Item * -include *.dir,*.vcxproj,*.vcxproj.filters,*.vcxproj.user,cmake_install.cmake,CMakeCache.txt,CMakeFiles,Debug,ipch,lib,MinSizeRel,Rainbow.sdf,Rainbow.sln,Release,RelWithDebInfo,Win32 -recurse
}
elseif ($Generate) {
	Make "$args"
}
elseif ($Help) {
	$Script = $MyInvocation.InvocationName
	Write-Host "Syntax: $Script [-Clean|-Generate [option ...]]"
	Write-Host
	Write-Host "Options:"
	Write-Host "  -DUNIT_TESTS=1           $UnitTestsDescription"
	Write-Host "  -DUSE_FMOD_STUDIO=1      $FMODDescription"
	Write-Host "  -DUSE_HEIMDALL=1         $HeimdallDescription"
	Write-Host "  -DUSE_PHYSICS=1          $PhysicsDescription"
	Write-Host "  -DUSE_SPINE=1            $SpineDescription"
	Write-Host "  -DUSE_VECTOR=1           $VectorDescription"
	Write-Host
	Write-Host "CMake options are passed directly to CMake so you can set variables like"
	Write-Host "-DCMAKE_BUILD_TYPE=<type> among others."
}
elseif ($Test) {
	Write-Host $PWD
	Write-Host $args
}
else {
	Add-Type -AssemblyName System.Windows.Forms

	$OutputFolder = ""
	while ([string]::IsNullOrEmpty($OutputFolder)) {
		$BuildFolder = $Shell.BrowseForFolder(0, "Please select an empty folder for building Rainbow", 0x1)
		if ([string]::IsNullOrEmpty($BuildFolder.Self.Path)) {
			exit
		}
		if (!$BuildFolder.Self.Path.StartsWith($SourcePath)) {
			$OutputFolder = $BuildFolder.Self.Path
			break
		}
		[void][System.Windows.Forms.MessageBox]::Show(
			"Please choose an empty folder outside the repository.",
			"Cannot build inside the repository.",
			[System.Windows.Forms.MessageBoxButtons]::OK,
			[System.Windows.Forms.MessageBoxIcon]::Information)
	}

	$UnitTestsCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $UnitTestsDescription
	}
	$FMODCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Checked = $true
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
	$SpineCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $SpineDescription
	}
	$VectorCheckBox = New-Object System.Windows.Forms.CheckBox -Property @{
		AutoSize = $true
		Text = $VectorDescription
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
		$SpineCheckBox,
		$VectorCheckBox,
		$ButtonLayout));

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
	if (!$FMODCheckBox.Checked) {
		$Options += "-DUSE_FMOD_STUDIO=0"
	}
	if ($HeimdallCheckBox.Checked) {
		$Options += "-DUSE_HEIMDALL=1"
	}
	if ($PhysicsCheckBox.Checked) {
		$Options += "-DUSE_PHYSICS=1"
	}
	if ($SpineCheckBox.Checked) {
		$Options += "-DUSE_SPINE=1"
	}
	if ($VectorCheckBox.Checked) {
		$Options += "-DUSE_VECTOR=1"
	}

	Push-Location $OutputFolder
	Make $Options
	Pop-Location
}
