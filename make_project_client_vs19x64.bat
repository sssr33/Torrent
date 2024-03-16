@echo off

call cmake -G "Visual Studio 16 2019" -A x64 -S ./client -B "!!BUILD_x64/client"
