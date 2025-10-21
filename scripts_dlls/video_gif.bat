
@echo off
REM Generate palette from frames
ffmpeg -i frame_%%d.png -vf "palettegen" palette.png

REM Use palette to create smooth GIF at 30 FPS
ffmpeg -framerate 30 -i frame_%%d.png -i palette.png -filter_complex "paletteuse" output.gif

pause
