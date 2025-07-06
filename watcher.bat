@echo off
watchexec --shell cmd -e cpp,h -r -- "mingw32-make -j8 debug DEBUG=true && .\\.build\\fortress.exe"
exit
