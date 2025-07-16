# $Rebuild: flag to force a rebuild
param(
	[switch]$Rebuild = $false
)

if ($Rebuild || !Test-Path -Path "build") {
	cmake -B build --log-level DEBUG || exit 1
}
cmake --build build --target gdcef gdcef-subprocess