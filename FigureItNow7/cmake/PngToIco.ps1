# -----------------------------------------------------------------------------
# GNU GENERAL PUBLIC LICENSE, version 3
# See LICENSE file for detail.
#
# Author: Yulong Yu
# Copyright(c) 2026 Yulong Yu. All rights reserved.
# -----------------------------------------------------------------------------
#
# Converts the application icon PNG (drawn by examples/icons/main.fsc and exported as
# icons/main.png) into a multi-size Windows .ico. Each size is stored as a PNG payload,
# which every Windows since Vista supports and which keeps the file small.
#
# Called from CMakeLists.txt (add_custom_command) and from FigureItNow7.pro (system()).

param(
    [Parameter(Mandatory = $true)][string]$SourcePNG,
    [Parameter(Mandatory = $true)][string]$OutputIco
)

Add-Type -AssemblyName System.Drawing

$src = [System.Drawing.Image]::FromFile($SourcePNG)
$sizes = @(256, 48, 32, 16)
$images = @()
foreach ($size in $sizes) {
    $bitmap = New-Object System.Drawing.Bitmap $size, $size
    $graphics = [System.Drawing.Graphics]::FromImage($bitmap)
    $graphics.InterpolationMode = [System.Drawing.Drawing2D.InterpolationMode]::HighQualityBicubic
    $graphics.PixelOffsetMode = [System.Drawing.Drawing2D.PixelOffsetMode]::HighQuality
    $graphics.DrawImage($src, 0, 0, $size, $size)
    $graphics.Dispose()
    $stream = New-Object System.IO.MemoryStream
    $bitmap.Save($stream, [System.Drawing.Imaging.ImageFormat]::Png)
    $bitmap.Dispose()
    $images += ,($stream.ToArray())
}
$src.Dispose()

$stream = [System.IO.File]::Create($OutputIco)
$writer = New-Object System.IO.BinaryWriter($stream)
$writer.Write([uint16]0)              # reserved
$writer.Write([uint16]1)              # resource type: icon
$writer.Write([uint16]$images.Count)  # number of directory entries

$offset = 6 + 16 * $images.Count
for ($i = 0; $i -lt $images.Count; $i++) {
    $size = $sizes[$i]
    $dimension = if ($size -eq 256) { 0 } else { $size }  # 256 is stored as 0
    $writer.Write([byte]$dimension)    # width
    $writer.Write([byte]$dimension)    # height
    $writer.Write([byte]0)             # palette size
    $writer.Write([byte]0)             # reserved
    $writer.Write([uint16]1)           # color planes
    $writer.Write([uint16]32)          # bits per pixel
    $writer.Write([uint32]$images[$i].Length)
    $writer.Write([uint32]$offset)
    $offset += $images[$i].Length
}
for ($i = 0; $i -lt $images.Count; $i++) { $writer.Write($images[$i]) }
$writer.Close()
