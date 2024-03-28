# SPDX-License-Identifier: GPL-3.0-or-later
#
# Copyright 2019 Kai Pastor (OpenOrienteering)
#
# This file is part of LibreMapper.

Add-Type -AssemblyName System.Device
$watcher = New-Object System.Device.Location.GeoCoordinateWatcher -arg 'High'

# Event handling

$statusChangedAction = {
	$status_ = $EventArgs.Status
	$line_ = "Status;$status_"
    $line_ | Write-Host
}
Register-ObjectEvent -InputObject $watcher -EventName StatusChanged -Action $statusChangedAction | Out-Null

$positionChangedAction = {
	$time_ = $EventArgs.Position.Timestamp.UtcDateTime
	$loc_ = $EventArgs.Position.Location
	$lat_ = $loc_.Latitude
	$lon_ = $loc_.Longitude
	$alt_ = $loc_.Altitude
	$hac_ = $loc_.HorizontalAccuracy
	$vac_ = $loc_.VerticalAccuracy
	# "" string interpretation does not use the culture, -f does.
	$line_ = "Position;{0:u};$lat_;$lon_;$alt_;$hac_;$vac_" -f ($time_)
    $line_ | Write-Host
}
Register-ObjectEvent -InputObject $watcher -EventName PositionChanged -Action $positionChangedAction | Out-Null

# Start

$watcher.start()

# Output permission status

$permission_ = $watcher.Permission
"Permission;$permission_" | Write-Host
