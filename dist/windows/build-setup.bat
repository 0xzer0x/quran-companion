"C:\Program Files\CMake\bin\cmake.exe" -G "Ninja" -S ../..  -B ./build
CD build
ninja

CD ..
MKDIR bundle
XCOPY ".\build\quran-companion.exe" ".\bundle\quran-companion.exe"
XCOPY ".\build\assets" ".\bundle\assets\" /E
XCOPY ".\build\audio" ".\bundle\audio\" /E
CD ./bundle

windeployqt.exe .\quran-companion.exe

CD ..
"C:\Program Files (x86)\Inno Setup 6\ISCC.exe" quran-companion-setup.iss